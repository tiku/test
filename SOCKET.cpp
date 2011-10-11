#include "SOCKET.h"
#include <stdio.h>

PairDataArray::PairDataArray(){
	m_pairs.clear();
}

PairData& PairDataArray::operator[](const int index){
	return m_pairs[index];
}

wchar_t* PairDataArray::operator[](const wchar_t*key){
	int index=Find(key);
	if(index==npos){
		return TEXT("");
	}
	return (wchar_t*)m_pairs[index].data.c_str();
}


bool PairDataArray::Insert(const wchar_t* key,const wchar_t* data){
	if(Find(key)!=npos){
		return false;
	}
	m_pairs.push_back(PairData(key,data));
	return true;
}

bool PairDataArray::Erase(const wchar_t* key){
	int index=Find(key);
	if(index==npos){
		return false;
	}
	return true;
}

int PairDataArray::Find(const wchar_t* key){
	int i;
	for(i=0;i<m_pairs.size();i++){
		if(m_pairs[i].key==key){
			return i;
		}
	}
	return npos;
}
		
int PairDataArray::Size(){
	return m_pairs.size();
}

void PairDataArray::Clear(){
	m_pairs.clear();
}

bool PairDataArray::Set(const wchar_t* key,const wchar_t* data){
	int index=Find(key);
	if(index==npos){
		return false;
	}
	m_pairs[index].data=data;
	return true;
}

void PairDataArray::Sort(){
	int i,j;
	const int size=Size();
	int gap,sort_cnt;
	bool done=false;

	gap=size;
	while(!done||1<gap){
		gap=max(gap/1.3,1);
		done=true;
	
		for(i=0;i<size-gap;i++){
			j=i+gap;
			if(m_pairs[i].key.compare(m_pairs[j].key)==1){
				std::swap(m_pairs[i],m_pairs[j]);
				done=false;
			}
		}
	}
}

Socket_Base::Socket_Base(){
	WSADATA wsadata;
	if(WSAStartup(WS_VERSION,&wsadata)!=0){
		MessageBox(NULL,TEXT("WINSOCKの初期化失敗"),NULL,MB_OK);
	}
	m_sock=NULL;
	m_host.clear();
}

Socket_Base::~Socket_Base(){
	Base_Reset();
	WSACleanup();
}

SOCKET Socket_Base::GetSocket(){
	return m_sock;
}

void Socket_Base::Base_Reset(){
	if(m_sock!=NULL){
		shutdown(m_sock,SD_BOTH);
		closesocket(m_sock);
		m_sock=NULL;
	}
	m_host.clear();
}

bool Socket_Base::Base_Connect(const wchar_t* host,const wchar_t* serv){
	ADDRINFOW hints;
	ADDRINFOW *addrinfo;
	wchar_t* port[17];//SHORTの最大値16bit+終端文字1bit;
	wchar_t* proto=TEXT("tcp");

	Base_Reset();

	memset(&hints,0,sizeof(hints));//必ず初期化する

	hints.ai_family=AF_INET;//AF_UNSPECでIPV6対応（めんどいのでしません。ただのメモです）
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_protocol=IPPROTO_TCP;
	GetAddrInfo(host,serv,&hints,&addrinfo);

	m_sock=socket(addrinfo->ai_family,addrinfo->ai_socktype,addrinfo->ai_protocol);
	if(m_sock==INVALID_SOCKET){
		goto EXIT;
	}

	if(connect(m_sock,addrinfo->ai_addr,addrinfo->ai_addrlen)==SOCKET_ERROR){
		goto EXIT;
	}
	m_host=host;
	FreeAddrInfo(addrinfo);
	return true;
EXIT://エラーの場合
	FreeAddrInfo(addrinfo);
	MessageBox(NULL,TEXT("ソケットの作成または接続に失敗しました"),NULL,MB_OK);
	exit(0);
	return false;
}


bool Socket_Base::Connect(const wchar_t* host){
	return Base_Connect(host,TEXT("http"));
}


int Socket_Base::Send(const wchar_t* w_send,int size){
	string m_send;
	WideToMultiChar(w_send,size,&m_send);
	return send(m_sock,m_send.c_str(),size,0);
}

int Socket_Base::Recv(wstring* res){
	char tmp[MAX_RECV]={0};
	int len,prev=0;
	wstring wtmp;

	res->clear();
	do{
		len=recv(m_sock,(char*)tmp,MAX_RECV,0);
		//勢い
		res->resize(len+prev);
		prev+=MultiByteToWideChar(CP_UTF8,NULL,tmp,len,(wchar_t*)res->c_str()+prev,len);
	}while(len!=0);
	return len;
}

void Socket_Base::Reset(){
	Base_Reset();
}


SSL_Socket::SSL_Socket(){
	ssl=NULL;
	ctx=NULL;
}

SSL_Socket::~SSL_Socket(){
	Reset();
}

void SSL_Socket::Reset(){
	Socket_Base::Base_Reset();
	if(ssl!=NULL){
		SSL_shutdown(ssl);
		SSL_free(ssl);
		ssl=NULL;
	}
	if(ctx!=NULL){
		SSL_CTX_free(ctx);
		ctx=NULL;
	}
}

bool SSL_Socket::Connect(const wchar_t* host){
	int i,err;
	
	Reset();
	Base_Connect(host,TEXT("https"));

	SSL_library_init();
	SSL_load_error_strings();

	ctx=SSL_CTX_new(SSLv23_client_method());
	i=SSL_CTX_load_verify_locations(ctx,NULL,"./BuiltinObjectToken-VerisignClass3PublicPrimaryCertificationAuthority-G2.cer");
	SSL_CTX_set_verify_depth(ctx,SSL_VERIFY_PEER);

	ssl=SSL_new(ctx);
	SSL_set_fd(ssl,m_sock);
	err=SSL_connect(ssl);
	if(err!=1){
		MessageBox(NULL,TEXT("SSL_connect Error"),TEXT("ERROR"),MB_OK);
		return false;
	}
	return true;
}

int SSL_Socket::Send(const wchar_t* w_send,int size){
	string m_send;
	WideToMultiChar(w_send,size,&m_send);
	return SSL_write(ssl,m_send.c_str(),m_send.size());
}

int SSL_Socket::Recv(wstring* res){
	
	int total_readed=0;
	res->clear();
	while(1){
		char buf[MAX_RECV];
		int read_size;

		read_size=SSL_read(ssl,buf,sizeof(buf)-1);
		res->resize(total_readed+read_size);
		total_readed+=MultiByteToWideChar(CP_UTF8,NULL,buf,read_size,(wchar_t*)res->c_str()+total_readed,read_size);
		if(read_size==(MAX_RECV-1)){
			buf[read_size]=0;
		}else{
			break;
		}
	}
	
	return true;
}


Socket::Socket(){
	m_socket=NULL;
}

Socket::~Socket(){
	Reset();
}

void Socket::Reset(){
	if(m_socket!=NULL){
		delete m_socket;
		m_socket=NULL;
	}
}

bool Socket::Connect(const wchar_t* host,const wchar_t* proto){
	Reset();

	if(wcscmp(proto,TEXT("http"))==0){
		m_socket=new Socket_Base();
	}else if(wcscmp(proto,TEXT("https"))==0){
		m_socket=new SSL_Socket();
	}
	return m_socket->Connect(host);
}

int Socket::Send(const wchar_t* send,int size){
	return m_socket->Send(send,size);
}

int Socket::Recv(wstring* res){
	return m_socket->Recv(res);
}


WSA_Async::WSA_Async(){
	m_socket=NULL;
	m_call=NULL;
	pause=false;
	running=false;
	m_hThread=NULL;
	m_hWSAEvent=NULL;
	m_hEvent=WSACreateEvent();
}

WSA_Async::~WSA_Async(){
	EventEnd();
	WSACloseEvent(m_hEvent);
}

void WSA_Async::Reset(){
	EventEnd();
	if(m_socket){
		delete m_socket;
		m_socket=NULL;
	}
}

bool WSA_Async::Connect(const wchar_t* host,const wchar_t* proto){
	Reset();
	if(wcscmp(proto,TEXT("http"))==0){
		m_socket=new Socket_Base();
	}else if(wcscmp(proto,TEXT("https"))==0){
		m_socket=new SSL_Socket();
	}else{
		MessageBox(NULL,TEXT("サポートしていません"),TEXT("CONNECT"),MB_OK);
	}
	return m_socket->Connect(host);
}

int WSA_Async::Send(const wchar_t* send,int size){
	return m_socket->Send(send,size);
}

int WSA_Async::Recv(ASYNC_CALLBACK call){
	ThreadStart(call);
	return 0;
}


unsigned int CALLBACK WSA_Async::CallProc(void* arg){
	return ((WSA_Async*)arg)->ThreadProc(NULL);
}

unsigned int CALLBACK WSA_Async::ThreadProc(void* arg){
	Event_Proc();
	_endthreadex(0);
	return 0;
}


void WSA_Async::Event_Proc(){
	enum{NETWORK_EVENT=0,SYSTEM_EVENT,SIZE};
	wstring res;
	WSANETWORKEVENTS events;
	DWORD dwResult;

	m_hWSAEvent=WSACreateEvent();
	WSAEventSelect(m_socket->GetSocket(),m_hWSAEvent,FD_READ|FD_CLOSE);

	HANDLE hWSAEvents[SIZE];
	hWSAEvents[NETWORK_EVENT]=m_hWSAEvent;
	hWSAEvents[SYSTEM_EVENT]=m_hEvent;

	while(running){
		dwResult=WSAWaitForMultipleEvents(SIZE,hWSAEvents,FALSE,WSA_INFINITE,FALSE);
		if(dwResult==WSA_WAIT_FAILED){
			break;
		}
		dwResult-=WSA_WAIT_EVENT_0;
		WSAResetEvent(hWSAEvents[dwResult]);

		if(dwResult==SYSTEM_EVENT){
			while(pause&&running){
				if(WSAWaitForMultipleEvents(1,&m_hEvent,FALSE,WSA_INFINITE,FALSE)==WSA_WAIT_FAILED){
					break;
				}
				WSAResetEvent(m_hEvent);
			}
		}else{
			wstring read;
			read.clear();
			WSAEnumNetworkEvents(m_socket->GetSocket(),m_hWSAEvent,&events);
			switch(events.lNetworkEvents){
			case FD_READ:
				m_socket->Recv(&read);
				m_call(events.lNetworkEvents,(wchar_t*)read.c_str());
				break;
			case FD_CLOSE:
				m_socket->Reset();
				m_call(events.lNetworkEvents,TEXT(""));
				running=false;
				break;
			}
		}
	}
END:
	running=false;
	pause=false;
	WSAResetEvent(m_hEvent);
	WSACloseEvent(m_hWSAEvent);
}

void WSA_Async::ThreadStart(ASYNC_CALLBACK call){
	if(running){
		return;
	}
	if(m_hThread){
		CloseHandle(m_hThread);
		m_hThread=NULL;
	}
	pause=false;
	running=true;
	m_call=call;
	m_hThread=(HANDLE)_beginthreadex(NULL,NULL,CallProc,this,NULL,NULL);
	return;
}

void WSA_Async::EventStart(){
	if(!running){
		return;
	}
	pause=false;
	WSASetEvent(m_hEvent);
}

void WSA_Async::EventStop(){
	if(!running){
		return;
	}
	pause=true;
	WSASetEvent(m_hEvent);
}

void WSA_Async::EventEnd(){
	if(!m_hThread){
		return;
	}
	if(running){
		running=false;
		SetEvent(m_hEvent);
		WaitForSingleObject(m_hThread,INFINITE);
	}
	CloseHandle(m_hThread);
	m_hThread=NULL;
}


bool Http_Func::GetRequestString(
	const wchar_t* cmd,
	const wchar_t* proto,
	const wchar_t* host,
	const wchar_t* path,
	const wchar_t* head,
	const wchar_t* content,
	wstring* res)
{
#define HTTP_VERSION_TEXT TEXT("HTTP/1.1")

	wstring url,s_content;
	url.append(proto).append(TEXT("://")).append(host).append(path);
	s_content=content;
	if((cmd==TEXT("GET")||cmd==TEXT("HEAD"))&&lstrlen(content)!=0){
		url.append(TEXT("?")).append(s_content);
		s_content.clear();
	}

	res->append(cmd).append(TEXT(" "));
	res->append(url);
	res->append(TEXT(" ")).append(HTTP_VERSION_TEXT).append(TEXT("\r\n"));
	res->append(TEXT("Host: ")).append(host).append(TEXT("\r\n"));
	res->append(head);
	res->append(TEXT("\r\n"));
	res->append(s_content);
	res->append(TEXT("\r\n\r\n"));
		
#undef HTTP_VERSION_TEXT
	return true;
}

bool Http_Func::GetRequestString(const wchar_t* cmd,const wchar_t* url,const wchar_t* head,const wchar_t* content,wstring*res){
#define HTTP_VERSION_TEXT TEXT("HTTP/1.1")
	wstring host,a,path;
	UrlSplit(url,&a,&host,&path);

	bool get=false;
	if(wcscmp(cmd,TEXT("GET"))==0||wcscmp(cmd,TEXT("HEAD"))==0){
		get=true;
	}
	res->clear();
	res->append(cmd).append(TEXT(" "));
	res->append(path);
	if(get&&lstrlen(content)!=0){
		res->append(TEXT("?")).append(content);
	}
	
	res->append(TEXT(" ")).append(HTTP_VERSION_TEXT).append(TEXT("\r\n"));

	res->append(TEXT("Host:")).append(host).append(TEXT("\r\n"));
	res->append(head);
	res->append(TEXT("\r\n"));
	if(!get){
		res->append(content);
	}
	res->append(TEXT("\r\n"));
#undef HTTP_VERSION_TEXT
	return true;
}

bool Http_Func::UrlSplit(const wchar_t*url,wstring*proto,wstring *host,wstring *path){
	const wchar_t *url_ptr,*find_ptr;
	int size;
	url_ptr=url;

	find_ptr=wcsstr(url_ptr,TEXT("://"));
	if(find_ptr==NULL){
		return false;
	}
	size=find_ptr-url_ptr;
	proto->assign(url_ptr,size);
	url_ptr+=size+lstrlen(TEXT("://"));

	find_ptr=wcschr(url_ptr,'/');
	if(find_ptr==NULL){
		return false;
	}
	size=find_ptr-url_ptr;
	host->assign(url_ptr,size);
	url_ptr+=size;

	find_ptr=wcschr(url_ptr,'?');
	if(find_ptr==NULL){
		path->assign(url_ptr);
		return true;
	}
	size=find_ptr-url_ptr;
	path->assign(url_ptr,size);
	return true;
}
//URLエンコード（Perlで書きたいなあ...ワイド文字使いたいなあ...）
bool Http_Func::UrlEncode(const wchar_t*url,wstring *res){
	int i,len;
	wstring dest;
	TCHAR ch;
	wchar_t tmp[4];
	len=lstrlen(url);
	dest.clear();
	//効率悪いような…
	for(i=0;i<len;i++){
		//そのまま
		ch=url[i];
		if(isalnum(ch)||ch=='_'||ch=='-'||ch=='.'){
			dest.append((wchar_t*)&ch,1);
		}else if(ch==' '){//空白は+へ変換
			dest.append(TEXT("+"));
		}else{//大変かとおもいきやsprintfでいけた
			wsprintf(tmp,TEXT("%%%02X"),ch);//(unsigned wchar_t*にしないとバグる)
			dest.append(tmp);
		}
	}
	res->assign(dest);
	return true;
}

void Http_Func::HeaderJoin(PairDataArray pairs,wstring *res){
	int i;
	res->clear();
	for(i=0;i<pairs.Size();i++){
		res->append(pairs[i].key).append(TEXT(":")).append(pairs[i].data).append(TEXT("\r\n"));
	}
	return;
}

void Http_Func::ContentJoin(PairDataArray pairs,wstring* res){
	int i;
	res->clear();
	for(i=0;i<pairs.Size();i++){
		res->append(pairs[i].key).append(TEXT("=")).append(pairs[i].data).append(TEXT("&"));
	}
	if(!res->empty()){
		res->erase(res->size()-1);
	}
	return;
}


Inet::Inet(){
	m_host=TEXT("");
	m_response=TEXT("");
}

Inet::~Inet(){};



bool Inet::Connect(const wchar_t* host,const wchar_t* proto){
	if(!m_socket.Connect(host,proto)){
		return false;
	}
	m_host=host;
	return true;
}


bool Inet::Async_Connect(const wchar_t* host){
	return true;
}

bool Inet::Request(const wchar_t* cmd,const wchar_t* url,PairDataArray head,PairDataArray content){
	wstring res;
	wstring  head_str,content_str;
	wstring proto,host,path;

	m_func.UrlSplit(url,&proto,&host,&path);
	Connect(host.c_str(),proto.c_str());

	SetDefaultHeader(&head);
	
	m_func.HeaderJoin(head,&head_str);
	m_func.ContentJoin(content,&content_str);

	m_func.GetRequestString(cmd,url,head_str.c_str(),content_str.c_str(),&res);
	m_socket.Send(res.c_str(),res.size());

	return true;
}

int Inet::Response(wstring* res){
	wstring status,head,content;
	int scode;
	PairDataArray head_data;
	m_socket.Recv(&m_response);
	ResponseSplit(m_response.c_str(),&status,&head,&content);
	
	*res=content;
	if(!StatusCodeAnalysis(status.c_str(),&scode)){
		return false;
	}
	if(!HeadAnalysis(head.c_str(),&head_data)){
		return false;
	}
	if(wcscmp(head_data[TEXT("Connection")],TEXT("Keep-Alive"))!=0){
		//Reset();
	}
	return scode;
}

bool Inet::ResponseSplit(const wchar_t* response,wstring* status,wstring* head,wstring* content){
	const wchar_t* end;
	const wchar_t* beg;

	beg=response;
	if((end=wcsstr(beg,TEXT("\r\n")))==NULL){
		return false;
	}
	status->assign(beg,end-beg);
	beg=end+lstrlen(TEXT("\r\n"));

	if((end=wcsstr(beg,TEXT("\r\n\r\n")))==NULL){
		return false;
	}
	head->assign(beg,end-beg);
	beg=end+lstrlen(TEXT("\r\n\r\n"));

	content->assign(beg);
	return true;
}

bool Inet::StatusCodeAnalysis(wstring sctext,int* res){
	int i,n,pos;
	wchar_t *err;
	if(pos=sctext.find(' ')==wstring::npos){
		return false;
	}
	sctext.erase(0,pos+1);
	if(pos=sctext.find(' ')==wstring::npos){
		return false;
	}
	sctext.erase(pos);
	*res=wcstol(sctext.c_str(),&err,10);
	if(lstrlen(err)!=0){
		return false;
	}
	return true;
}

bool Inet::HeadAnalysis(const wchar_t* hdtext,PairDataArray*res){
	const wchar_t* ptr,*p;
	int i,pos;
	const wchar_t *beg,*end;
	PairData pair;
	PairDataArray res_tmp;

	if(lstrlen(hdtext)==0){
		return true;
	}
	res->Clear();
	ptr=hdtext;
	beg=hdtext;
	do{
		pair.Set(TEXT(""),TEXT(""));

		if((end=wcsstr(beg,TEXT(":")))==NULL){
			return false;
		}
		pair.key.assign(beg,end-beg);
		beg=end+lstrlen(TEXT(":"));

		if((end=wcsstr(beg,TEXT("\r\n")))==NULL){
			return false;
		}
		pair.data.assign(beg,end-beg);
		beg=end+lstrlen(TEXT("\r\n"));

		while(pair.key[max(0,pair.key.size()-1)]==' '){
			pair.key.erase(pair.key.size()-1);
		}
		while(pair.data[0]==' '){
			pair.data.assign(pair.data,1,pair.data.size());
		}

		res_tmp.Insert(pair.key.c_str(),pair.data.c_str());
	}while(wcsncmp(beg,TEXT("\r\n"),lstrlen(TEXT("\r\n")))!=0&&beg!='\0');
	*res=res_tmp;
	return true;
}

bool Inet::SetDefaultHeader(PairDataArray* head){
	head->Insert(TEXT("Connection"),TEXT("Keep-Alive"));
	head->Insert(TEXT("User-Agent"),TEXT("Inet/2"));
	head->Insert(TEXT("Accept-Language"),TEXT("ja"));
	return true;
}


int Inet::Auto(const wchar_t*cmd,const wchar_t*url,PairDataArray head_pair,PairDataArray content_pair,wstring* content_res){
	Request(cmd,url,head_pair,content_pair);
	int sc_res=Response(content_res);
	return sc_res;
	//return 0;
}


/*
void Inet_Async::Auto(const wchar_t* cmd,const wchar_t* url,PairDataArray head,PairDataArray content,unsigned int CALLBACK cb(void*),void* arg){
	unsigned int res;
	inet.Auto(cmd,url,head,content,NULL);

}
	
void Inet_Async::Create(unsigned int CALLBACK cb(void*),void* arg){
	unsigned int res;
	hThread=(HANDLE)_beginthreadex(NULL,NULL,CallProc,this,NULL,&res);
}

unsigned int CALLBACK Inet_Async::CallProc(void* arg){
	return ((Inet_Async*)arg)->ThreadProc(NULL);
}

unsigned int CALLBACK Inet_Async::ThreadProc(void* arg){
	/*wstring res;
	HANDLE hEvent;
	WSANETWORKEVENTS events;

	hEvent=WSACreateEvent();
	WSAEventSelect(m_socket.GetSocket(),hEvent,FD_READ|FD_CONNECT|FD_CLOSE);
	while(running){
		if(WSAWaitForMultipleEvents(1,&hEvent,FALSE,WSA_INFINITE,FALSE)==WSA_WAIT_FAILED){
			break;
		}
		WSAEnumNetworkEvents(m_socket.GetSocket(),hEvent,&events);
		call(events.lNetworkEvents);
	}
	WSACloseEvent(hEvent);
	_endthreadex(0);
	return 0;
}
*/
/*
bool Async::Start(ASYNC_CALLBACK cb){
	unsigned int res;
	call=cb;
	hThread=(HANDLE)_beginthreadex(NULL,NULL,Callback,(LPVOID)this,NULL,&res);
	return true;
}

bool Async::End(){
	delete this;
	return true;
}

THREAD_CALL Async::Callback(void* ar){
	return ((Async*)ar)->Async_func(ar);
}

THREAD_CALL CALLBACK Async::Async_func(void* call){
	((Async*)call)->call(&inet);
	End();
	return 0;
}
*/
/*
bool Async::Start(){
	m_socket=socket;
	return true;
}

unsigned int CALLBACK Async::Async_Call(void*){
}

unsigned int CALLBACK Async::Async_func(void*){
	HANDLE hEvent;
	WSANETWORKEVENTS events;

	hEvent=WSACreateEvent();
	WSAEventSelect(m_socket.GetSocket(),hEvent,FD_READ|FD_CONNECT|FD_CLOSE);
	while(1){
		if(WSAWaitForMultipleEvents(1,&hEvent,FALSE,WSA_INFINITE,FALSE)==WSA_WAIT_FAILED){
			break;
		}
		WSAEnumNetworkEvents(m_socket.GetSocket(),hEvent,&events);
		switch(events.lNetworkEvents){
		case FD_CONNECT:
			break;
		case FD_CLOSE:
			break;
		case FD_READ:
			break;
		}
	}
	WSACloseEvent(hEvent);
	_endthreadex(0);
	CloseHandle(hThread);
}
*/