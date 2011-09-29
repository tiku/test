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
	Com_Reset();
	WSACleanup();
}

SOCKET Socket_Base::GetSocket(){
	return m_sock;
}

void Socket_Base::Com_Reset(){
	if(m_sock!=NULL){
		shutdown(m_sock,SD_BOTH);
		closesocket(m_sock);
		m_sock=NULL;
	}
	m_host.clear();
}

bool Socket_Base::Com_Connect(const wchar_t* host,const wchar_t* serv){
	ADDRINFOW hints;
	ADDRINFOW *addrinfo;
	wchar_t* port[17];//SHORTの最大値16bit+終端文字1bit;
	wchar_t* proto=TEXT("tcp");

	Com_Reset();

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


bool Def_Socket::Connect(const wchar_t* host){
	return Com_Connect(host,TEXT("http"));
}


int Def_Socket::Send(const wchar_t* w_send,int size){
	string m_send;
	WideToMultiChar(w_send,size,&m_send);
	return send(m_sock,m_send.c_str(),size,0);
}

int Def_Socket::Recv(wstring* res){
	char tmp[MAX_RECV]={0};
	int len,prev=0;
	wstring wtmp;

	res->clear();
	do{
		len=recv(m_sock,(char*)tmp,MAX_RECV,0);
		//勢い
		res->resize(len+prev);
		prev+=MultiByteToWideChar(CP_UTF8,NULL,tmp,len,(wchar_t*)res->c_str()+prev,len);
		memset(&tmp,0,sizeof(tmp));
	}while(len==MAX_RECV);
	return len;
}

SSL_Socket::SSL_Socket(){
	ssl=NULL;
	ctx=NULL;
}

SSL_Socket::~SSL_Socket(){
	SSL_Reset();
}

void SSL_Socket::SSL_Reset(){
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
	Com_Connect(host,TEXT("https"));

	SSL_Reset();

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
		m_socket=new Def_Socket();
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



Inet_Async::~Inet_Async(){
	running=false;
	WSASetEvent(0);
	WaitForSingleObject(m_hThread,INFINITE);
	//WaitForSingleObject(m_hThread,INFINITE);
}

bool Inet_Async::Connect(const wchar_t* host,const wchar_t* proto){
	m_socket=new SSL_Socket;
	return m_socket->Connect(host);
}

bool Inet_Async::Request(const wchar_t* cmd,const wchar_t* url,PairDataArray head,PairDataArray content){
	Http_Func func;
	wstring proto,host,path;
	wstring head_str,content_str,send;

	func.UrlSplit(url,&proto,&host,&path);

	Connect(host.c_str(),proto.c_str());
	func.HeaderJoin(head,&head_str);
	func.ContentJoin(content,&content_str);

	func.GetRequestString(cmd,url,head_str.c_str(),content_str.c_str(),&send);
	m_socket->Send(send.c_str(),send.size());
	return true;
}


void Inet_Async::Start(){
	unsigned int res;
	if(suspend){
		SetEvent(hEv2);
		suspend=false;
		return;
	}
	running=true;

	hEv2=CreateEvent(NULL,TRUE,FALSE,TEXT("AAA"));
	Function();
//	m_hThread=(HANDLE)_beginthreadex(NULL,NULL,CallProc,this,NULL,&res);
	return;
}

void Inet_Async::Stop(){
	SetEvent(hEv2);
	suspend=true;
	return;
	running=false;
	WSASetEvent(0);
	WaitForSingleObject(m_hThread,INFINITE);
}

void Inet_Async::End(){

}

unsigned int CALLBACK Inet_Async::CallProc(void* arg){
	return ((Inet_Async*)arg)->ThreadProc(arg);
}

unsigned int CALLBACK Inet_Async::ThreadProc(void* arg){
	wstring res;
	//HANDLE hEvent;
	WSANETWORKEVENTS events;

	hEvent=WSACreateEvent();
	WSAEventSelect(m_socket->GetSocket(),hEvent,FD_READ|FD_CONNECT|FD_CLOSE);
	while(running){
		if(WSAWaitForMultipleEvents(1,&hEvent,FALSE,WSA_INFINITE,FALSE)==WSA_WAIT_FAILED){
			break;
		}
		WSAEnumNetworkEvents(m_socket->GetSocket(),hEvent,&events);
		switch(events.lNetworkEvents){
		case FD_CONNECT:
			break;
		case FD_CLOSE:
			break;
		case FD_READ:
			m_socket->Recv(&res);
			break;
		}
	}
	WSACloseEvent(hEvent);

//	CloseHandle(m_hThread);
//	_endthreadex(0);
	return true;
}

void Inet_Async::Function(){
	wstring res;
	//HANDLE hEvent;
	WSANETWORKEVENTS events;

	hEvent=WSACreateEvent();
	WSAEventSelect(m_socket->GetSocket(),hEvent,FD_READ|FD_CLOSE);

	suspend=false;
	while(running){
		if(suspend){
			WaitForSingleObject(hEv2,INFINITE);
			continue;
		}
		if(WSAWaitForMultipleEvents(1,&hEvent,FALSE,WSA_INFINITE,FALSE)==WSA_WAIT_FAILED){
			break;
		}
		WSAEnumNetworkEvents(m_socket->GetSocket(),hEvent,&events);
		switch(events.lNetworkEvents){
		case FD_CLOSE:
			break;
		case FD_READ:
			m_socket->Recv(&res);
			break;
		}
	}
	WSACloseEvent(hEvent);
}

Inet_Async_Create::~Inet_Async_Create(){
}


void Inet_Async_Create::Create(const wchar_t* cmd,const wchar_t* url,PairDataArray head,PairDataArray content){
	unsigned int res;
	a=cmd;
	b=url;
	c=head;
	d=content;
	//ia.Request(a.c_str(),b.c_str(),c,d);
	//ia.Start();
	m_hThread=(HANDLE)_beginthreadex(NULL,NULL,cb,this,NULL,&res);
}

void Inet_Async_Create::Stop(){
	ia.Stop();
	//SuspendThread(m_hThread);
}

void Inet_Async_Create::Start(){
	ia.Start();
}


unsigned int Inet_Async_Create::cb(void* arg){
	return ((Inet_Async_Create*)arg)->proc(NULL);
}

unsigned int Inet_Async_Create::proc(void* arg){
	ia.Request(a.c_str(),b.c_str(),c,d);
	ia.Start();
	CloseHandle(m_hThread);
	_endthreadex(0);
	return 0;
}

unsigned int CALLBACK WSA_Async::CallProc(void* arg){
	return ((WSA_Async*)arg)->ThreadProc(NULL);
}

unsigned int CALLBACK WSA_Async::ThreadProc(void* arg){
	Event_Proc();

	CloseHandle(m_hThread);
	_endthreadex(0);
	return 0;
}


void WSA_Async::Event_Proc(){
	wstring res;
	WSANETWORKEVENTS events;
	DWORD dwResult;

	m_hWSAEvent=WSACreateEvent();
	WSAEventSelect(m_socket->GetSocket(),m_hWSAEvent,FD_READ|FD_CLOSE);
	HANDLE hWSAEvents[3];
	hWSAEvents[0]=m_hWSAEvent;
	hWSAEvents[1]=m_hWSAStop;
	hWSAEvents[2]=m_hWSAExit;
	while(1){
		dwResult=WSAWaitForMultipleEvents(3,hWSAEvents,FALSE,WSA_INFINITE,FALSE);
		if(dwResult==WSA_WAIT_FAILED){
			break;
		}
		if(dwResult-WSA_WAIT_EVENT_0==2){
			break;
		}
		if(dwResult-WSA_WAIT_EVENT_0==1){
			WSAResetEvent(m_hWSAStop);
			if(WSAWaitForMultipleEvents(1,&m_hWSAStop,FALSE,WSA_INFINITE,FALSE)==WSA_WAIT_FAILED){
				break;
			}
		}
		if(dwResult-WSA_WAIT_EVENT_0==0){
			WSAEnumNetworkEvents(m_socket->GetSocket(),m_hWSAEvent,&events);
			switch(events.lNetworkEvents){
			case FD_CLOSE:
				break;
			case FD_READ:
				m_socket->Recv(&res);
				break;
			}
		}
	}
	WSACloseEvent(m_hWSAEvent);
}

void WSA_Async::Request(const wchar_t* cmd,const wchar_t* url,PairDataArray head,PairDataArray content){
	Http_Func func;
	wstring head_str,content_str,send;
	wstring proto,host,path;
	
	func.UrlSplit(url,&proto,&host,&path);

	m_socket=new SSL_Socket();
	m_socket->Connect(host.c_str());

	func.HeaderJoin(head,&head_str);
	func.ContentJoin(content,&content_str);
	func.GetRequestString(cmd,url,head_str.c_str(),content_str.c_str(),&send);
	m_socket->Send(send.c_str(),send.size());
}

void WSA_Async::Response(){
	//Event_Proc();
	m_hThread=(HANDLE)_beginthreadex(NULL,NULL,CallProc,this,NULL,NULL);
}

void WSA_Async::Start(){
}

void WSA_Async::Stop(){
	WSASetEvent(m_hWSAStop);
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
	
	if(!StatusCodeAnalysis(status.c_str(),&scode)){
		return false;
	}
	if(!HeadAnalysis(head.c_str(),&head_data)){
		return false;
	}
	if(wcscmp(head_data[TEXT("Connection")],TEXT("Keep-Alive"))!=0){
		//Reset();
	}
	*res=content;
	return scode;
}

bool Inet::ResponseSplit(const wchar_t* response,wstring* status,wstring* head,wstring* content){
	const wchar_t* resp;
	int size;

	resp=response;
	size=(wcsstr(resp,TEXT("\r\n"))-resp)+lstrlen(TEXT("\r\n"));
	status->assign(resp,size);

	resp+=size;
	size=(wcsstr(resp,TEXT("\r\n\r\n"))-resp)+lstrlen(TEXT("\r\n\r\n"));
	head->assign(resp,size);

	resp+=size;
	content->assign(resp);
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
	PairData pair;
	PairDataArray res_tmp;

	if(lstrlen(hdtext)==0){
		return true;
	}
	res->Clear();
	ptr=hdtext;
	do{
		pair.Set(TEXT(""),TEXT(""));
		if((p=wcsstr(ptr,TEXT(":")))==NULL){
			return false;
		}
		pos=p-ptr;
		pair.key.assign(ptr,pos);
		ptr+=pos+lstrlen(TEXT(":"));

		if((p=wcsstr(ptr,TEXT("\r\n")))==NULL){
			return false;
		}
		pos=p-ptr;
		pair.data.assign(ptr,pos);
		ptr+=pos+lstrlen(TEXT("\r\n"));

		while(pair.key[max(0,pair.key.size()-1)]==' '){
			pair.key.erase(pair.key.size()-1);
		}
		while(pair.data[0]==' '){
			pair.data.assign(pair.data,1,pair.data.size());
		}

		res_tmp.Insert(pair.key.c_str(),pair.data.c_str());
	}while(wcsncmp(ptr,TEXT("\r\n"),lstrlen(TEXT("\r\n")))!=0);
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