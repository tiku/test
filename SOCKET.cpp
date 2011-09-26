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


Socket::Socket(){
	Init();
}
Socket::~Socket(){
	End();
}

bool Socket::Init(){
	WSADATA WsaData;
	
	if(WSAStartup(WS_VERSION,&WsaData)!=0){
		MessageBox(NULL,TEXT("WINSOCK初期化に失敗しました"),NULL,MB_OK);
		return false;
	}
	m_sock=NULL;
	m_host.clear();

	return true;
}

bool Socket::Reset(){
	if(m_sock!=NULL){
		closesocket(m_sock);
		m_sock=NULL;
		m_host.clear();
	}
	return true;
}

bool Socket::End(){
	Reset();
	WSACleanup();
	return true;
}

bool Socket::Connect(const wchar_t* host,const wchar_t* serv){
	ADDRINFOW hints;
	ADDRINFOW *addrinfo;
	wchar_t* port[17];//SHORTの最大値16bit+終端文字1bit;
	wchar_t* proto=TEXT("tcp");

	Reset();

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

SOCKET Socket::GetSocket(){
	return m_sock;
}

wchar_t* Socket::GetHost(){
	return (wchar_t*)m_host.c_str();
}

int Socket::Send(const wchar_t* msg,int length,int flag){
	string sendstr;
	WideToMultiChar(msg,length,&sendstr);
	return send(m_sock,(char*)sendstr.c_str(),length,flag);
}

int Socket::Recv(wstring* res,int flag){
	char tmp[MAX_RECV]={0};
	wstring::size_type rlen;
	int len,prev=0;
	rlen=0;
	wstring wtmp;
	res->clear();
	do{
		len=recv(m_sock,(char*)tmp,MAX_RECV,flag);
		//勢い
		res->resize(len+prev);
		prev+=MultiByteToWideChar(CP_UTF8,NULL,tmp,len,(wchar_t*)res->c_str()+prev,len);
		//MultiToWideChar(tmp,len,&wtmp);
		//res->append(wtmp);
		//res->resize(rlen+MultiByteToWideChar(CP_UTF8,NULL,tmp,MAX_RECV,0,0)+1);
		//rlen+=MultiByteToWideChar(CP_UTF8,NULL,tmp,MAX_RECV,(wchar_t*)res->c_str()+rlen,res->capacity()-rlen);
		memset(&tmp,0,sizeof(tmp));
	}while(len==MAX_RECV);
	return len;
}
/*
bool Http::UrlSplit(const wchar_t*url,wstring*proto,wstring *host,wstring *path){
	wchar_t* pcp;
	wchar_t *cproto,*chost,*cpath;
	cproto=chost=wcsstr((wchar_t*)url,TEXT("://"));
	chost+=lstrlen(TEXT("://"));
	cpath=wcschr(chost,'/');
	
	proto->assign(url,cproto-url);
	host->assign(chost,cpath-chost);
	path->assign(cpath);
	return true;
}
//URLエンコード（Perlで書きたいなあ...ワイド文字使いたいなあ...）
bool Http::UrlEncode(const wchar_t*url,wstring *res){
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

void Http::GetHeadStr(vector<PairData> head,wstring *res){
	int i;
	res->clear();
	for(i=0;i<head.size();i++){
		res->append(head[i].key).append(TEXT(":")).append(head[i].data).append(TEXT("\r\n"));
	}
	return;
}

void Http::GetContentStr(vector<PairData> content,wstring *res){
	int i;
	res->clear();
	for(i=0;i<content.size();i++){
		res->append(content[i].key).append(TEXT("=")).append(content[i].data).append(TEXT("&"));
	}
	return;
}


bool Http::Send(const wchar_t* cmd,const wchar_t* url,vector<PairData> head,vector<PairData> content){
	int i,clen;
	PairData host_head,connect_head,contlen_head,ua;
	wstring proto,host,path;
	wstring send_str,head_str,content_str;

	UrlSplit(url,&proto,&host,&path);
	UrlEncode(host.c_str(),&host);
	UrlEncode(path.c_str(),&path);
	//if(m_host!=host){
		if(!m_socket.Connect(host.c_str(),proto.c_str())){
			return false;
		}
	//}
	m_host=host;
	send_str.append(cmd).append(TEXT(" ")).append(url).append(TEXT(" HTTP/1.1\r\n"));
	m_socket.Send(send_str.c_str(),send_str.size(),0);

	head.insert(head.begin(),host_head.Set(TEXT("Host"),host.c_str()));
	GetHeadStr(head,&head_str);
	m_socket.Send(head_str.c_str(),head_str.size(),NULL);

	GetContentStr(content,&content_str);
	m_socket.Send(content_str.c_str(),content_str.size(),NULL);

	send_str.assign(TEXT("\r\n\r\n"));
	m_socket.Send(send_str.c_str(),send_str.size(),0);
	return true;
}

bool Http::Recv(wstring *status,wstring* header,wstring* body){
	int i,delim_index,begin,end;
	wstring res;
	m_socket.Recv(&res,0);
	begin=0;
	end=res.find(TEXT("\r\n"),begin)+lstrlen(TEXT("\r\n"));
	status->append(res,begin,end-begin);
	begin=end;
	end=res.find(TEXT("\r\n\r\n"),begin)+lstrlen(TEXT("\r\n\r\n"));
	header->append(res,begin,end-begin);
	begin=end;
	end=res.size();
	body->append(res,begin,end-begin);
	
	return true;
}
*/


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
	wstring host,a,b;
	bool get=false;
	if(cmd==TEXT("GET")||cmd==TEXT("HEAD")){
		get=true;
	}
	res->clear();
	res->append(cmd).append(TEXT(" "));
	res->append(url);
	if(get&&lstrlen(content)!=0){
		res->append(TEXT("?")).append(content);
	}
	
	res->append(TEXT(" ")).append(HTTP_VERSION_TEXT).append(TEXT("\r\n"));
	UrlSplit(url,&a,&host,&b);
	res->append(TEXT("Host :")).append(host).append(TEXT("\r\n"));
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

const wchar_t* Http_Bs::protocol=TEXT("http");

bool Http_Bs::Connect(const wchar_t *host){
	m_socket.Connect(host,GetProtocol());
	m_host=host;
	return true;
}

bool Http_Bs::Send(const wchar_t* msg,int size){
	m_socket.Send(msg,size,0);
	return true;
}

bool Http_Bs::Recv(wstring* res){
	m_socket.Recv(res,0);
	return true;
}

wchar_t* Http_Bs::GetProtocol(){
	return TEXT("http");
}

wchar_t* Http_Bs::Protocol(){
	return TEXT("http");
}

bool Https_Bs::Connect(const wchar_t *host){
	int i,err,s;
	DWORD res;
	long VeryErr;

	m_socket.Connect(host,GetProtocol());
	m_host=host;

	SSL_library_init();
	SSL_load_error_strings();

	ctx=SSL_CTX_new(SSLv23_client_method());
	i=SSL_CTX_load_verify_locations(ctx,NULL,"./BuiltinObjectToken-VerisignClass3PublicPrimaryCertificationAuthority-G2.cer");
	SSL_CTX_set_verify_depth(ctx,SSL_VERIFY_PEER);

	ssl=SSL_new(ctx);
	SSL_set_fd(ssl,m_socket.GetSocket());
	err=SSL_connect(ssl);
	if(err!=1){
		MessageBox(NULL,TEXT("SSL_connect Error"),TEXT("ERROR"),MB_OK);
		return false;
	}
	/*
	X509 *server_cert;
	char msg[1000];
	char *str;

	SSL_SESSION *ssi=SSL_get_session(ssl);

	server_cert=SSL_get_peer_certificate(ssl);

	str=X509_NAME_oneline(X509_get_subject_name(server_cert),NULL,NULL);

	sprintf(msg,"Subject:%s",str);
	MessageBoxA(NULL,msg,NULL,MB_OK);

	str=X509_NAME_oneline(X509_get_issuer_name(server_cert),NULL,NULL);

	sprintf(msg,"Issuer:%s",str);
	MessageBoxA(NULL,msg,NULL,MB_OK);

	VeryErr=SSL_get_verify_result(ssl);
	
	if(VeryErr==X509_V_OK){
		MessageBoxA(NULL,"Verify_OK!",NULL,MB_OK);
	}else{
		sprintf(msg,"%d",VeryErr);

		MessageBoxA(NULL,msg,NULL,MB_OK);
	}
	*/
	return true;
}

bool Https_Bs::Send(const wchar_t* msg,int size){
	string m_msg;
	WideToMultiChar(msg,size,&m_msg);
	SSL_write(ssl,m_msg.c_str(),m_msg.size());
	return true;
}

bool Https_Bs::Recv(wstring *res){
	int total_readed=0;
	res->clear();
	while(1){
		char buf[100];
		int read_size;

		read_size=SSL_read(ssl,buf,sizeof(buf)-1);
		res->resize(total_readed+read_size);
		total_readed+=MultiByteToWideChar(CP_UTF8,NULL,buf,read_size,(wchar_t*)res->c_str()+total_readed,read_size);
		if(read_size>0){
			buf[read_size]=0;
		}else{
			break;
		}
	}
	return true;
}

wchar_t* Https_Bs::GetProtocol(){
	return TEXT("https");
}

wchar_t* Https_Bs::Protocol(){
	return TEXT("https");
}

Inet::Inet(){
	Init();
}

Inet::~Inet(){
	Reset();
}

void Inet::Init(){
	m_http=NULL;
	m_host=TEXT("");
	m_response=TEXT("");
}


void Inet::Reset(){
	if(m_http!=NULL){
		delete m_http;
	}
	Init();
}

bool Inet::Connect(const wchar_t* host,const wchar_t* proto){

	if(wcscmp(proto,Http_Bs::Protocol())==0){
		m_http=new Http_Bs;
	}else if(wcscmp(proto,Https_Bs::Protocol())==0){
		m_http=new Https_Bs;
	}
	m_http->Connect(host);
	m_host=host;
	return true;
}


bool Inet::Async_Connect(const wchar_t* host){
	return true;
}

bool Inet::Request(const wchar_t* cmd,const wchar_t* path,const wchar_t* head,const wchar_t* content){
	wstring res;
	m_func.GetRequestString(cmd,m_http->GetProtocol(),m_host.c_str(),path,head,content,&res);
	m_http->Send(res.c_str(),res.size());

	return true;
}

int Inet::Response(wstring* res){
	wstring status,head,content;
	int scode;
	PairDataArray head_data;
	m_http->Recv(&m_response);
	ResponseSplit(m_response.c_str(),&status,&head,&content);
	
	if(!StatusCodeAnalysis(status.c_str(),&scode)){
		return false;
	}
	if(!HeadAnalysis(head.c_str(),&head_data)){
		return false;
	}
	if(wcscmp(head_data[TEXT("Connection")],TEXT("Keep-Alive"))!=0){
		Reset();
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
	head->Insert(TEXT("User-Agent"),TEXT("Inet /2"));
	head->Insert(TEXT("Accept-Language"),TEXT("ja"));
	return true;
}


int Inet::Auto(const wchar_t*cmd,const wchar_t*url,PairDataArray head_pair,PairDataArray content_pair,wstring* content_res){
	wstring proto,host,path,head_str,content_str;
	m_func.UrlSplit(url,&proto,&host,&path);

	if(m_host!=host.c_str()){
		Connect(host.c_str(),proto.c_str());
	}
	SetDefaultHeader(&head_pair);
	m_func.HeaderJoin(head_pair,&head_str);
	m_func.ContentJoin(content_pair,&content_str);
	Request(cmd,path.c_str(),head_str.c_str(),content_str.c_str());
	int sc_res=Response(content_res);
	return sc_res;
}

bool Http::UrlSplit(const wchar_t*url,wstring*proto,wstring *host,wstring *path){
	wchar_t* pcp;
	wchar_t *cproto,*chost,*cpath;
	cproto=wcsstr((wchar_t*)url,TEXT("://"));
	if(cproto==NULL){
		return false;
	}
	
	chost=cproto+lstrlen(TEXT("://"));
	cpath=wcschr(chost,'/');
	
	proto->assign(url,cproto-url);
	host->assign(chost,cpath-chost);
	path->assign(cpath);
	return true;
}
//URLエンコード（Perlで書きたいなあ...ワイド文字使いたいなあ...）←ワイド文字使いました＾＾
bool Http::UrlEncode(const wchar_t*url,wstring *res){
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

void Http::GetHeadStr(vector<PairData> head,wstring *res){
	int i;
	res->clear();
	for(i=0;i<head.size();i++){
		res->append(head[i].key).append(TEXT(":")).append(head[i].data).append(TEXT("\r\n"));
	}
	return;
}

void Http::GetContentStr(vector<PairData> content,wstring *res){
	int i;
	res->clear();
	for(i=0;i<content.size();i++){
		res->append(content[i].key).append(TEXT("=")).append(content[i].data).append(TEXT("&"));
	}
	return;
}


bool Http::Send(const wchar_t* cmd,const wchar_t* url,vector<PairData> head,vector<PairData> content){
	int i,clen;
	PairData host_head,connect_head,contlen_head,ua;
	wstring proto,host,path;
	wstring send_str,head_str,content_str;

	UrlSplit(url,&proto,&host,&path);
	UrlEncode(host.c_str(),&host);
	UrlEncode(path.c_str(),&path);
	//if(m_host!=host){
		if(!m_socket.Connect(host.c_str(),proto.c_str())){
			return false;
		}
	//}
	m_host=host;
	send_str.append(cmd).append(TEXT(" ")).append(url).append(TEXT(" HTTP/1.1\r\n"));
	m_socket.Send(send_str.c_str(),send_str.size(),0);

	head.insert(head.begin(),host_head.Set(TEXT("Host"),host.c_str()));
	GetHeadStr(head,&head_str);
	m_socket.Send(head_str.c_str(),head_str.size(),NULL);

	GetContentStr(content,&content_str);
	m_socket.Send(content_str.c_str(),content_str.size(),NULL);

	send_str.assign(TEXT("\r\n\r\n"));
	m_socket.Send(send_str.c_str(),send_str.size(),0);
	return true;
}

bool Http::Recv(wstring *status,wstring* header,wstring* body){
	int i,delim_index,begin,end;
	wstring res;
	m_socket.Recv(&res,0);
	begin=0;
	end=res.find(TEXT("\r\n"),begin)+lstrlen(TEXT("\r\n"));
	status->append(res,begin,end-begin);

	begin=end;
	end=res.find(TEXT("\r\n\r\n"),begin)+lstrlen(TEXT("\r\n\r\n"));
	header->append(res,begin,end-begin);

	begin=end;
	end=res.size();
	body->append(res,begin,end-begin);
	
	return true;
}

bool Http::Response(wstring *res){
	m_socket.Recv(res,NULL);
	return true;
}

bool Http::Request(const wchar_t* cmd,const wchar_t* path,const wchar_t* head,const wchar_t* content){
	wstring req;
	func.GetRequestString(cmd,TEXT("http"),m_host.c_str(),path,head,content,&req);
	m_socket.Send(req.c_str(),req.size(),NULL);
	return true;
}


bool OpenSSL::Connect(const wchar_t* host){
	int i,err,s;
	DWORD res;
	long VeryErr;
	Init();
	//SSL_CTX *ctx;
	//SSL *ssl;
	

	m_socket.Connect(host,TEXT("https"));
	/*
	if(m_bAsync){
		HANDLE hEvent;
		WSANETWORKEVENTS events;

		hEvent=WSACreateEvent();
		WSAEventSelect(m_socket.GetSocket(),hEvent,FD_READ|FD_CONNECT|FD_CLOSE);
		WSAWaitForMultipEvents(1,&hEvent,FALSE,SOCKET_CONNECT_TIMEOUT,FALSE);
		WSAEnumNetworkEvents(m_socket.GetSocket(),hEvent,&events);
		/*fd_set fds;
		FD_ZERO(&fds);
		FD_SET(m_socket.GetSocket(),&fds);*/
	//}

	SSL_library_init();
	SSL_load_error_strings();

	ctx=SSL_CTX_new(SSLv23_client_method());
	i=SSL_CTX_load_verify_locations(ctx,NULL,"./BuiltinObjectToken-VerisignClass3PublicPrimaryCertificationAuthority-G2.cer");
	SSL_CTX_set_verify_depth(ctx,SSL_VERIFY_PEER);

	ssl=SSL_new(ctx);
	SSL_set_fd(ssl,m_socket.GetSocket());
	err=SSL_connect(ssl);
	if(err!=1){
		MessageBox(NULL,TEXT("SSL_connect Error"),TEXT("ERROR"),MB_OK);
		return false;
	}
	/*
	X509 *server_cert;
	char msg[1000];
	char *str;

	SSL_SESSION *ssi=SSL_get_session(ssl);

	server_cert=SSL_get_peer_certificate(ssl);

	str=X509_NAME_oneline(X509_get_subject_name(server_cert),NULL,NULL);

	sprintf(msg,"Subject:%s",str);
	MessageBoxA(NULL,msg,NULL,MB_OK);

	str=X509_NAME_oneline(X509_get_issuer_name(server_cert),NULL,NULL);

	sprintf(msg,"Issuer:%s",str);
	MessageBoxA(NULL,msg,NULL,MB_OK);

	VeryErr=SSL_get_verify_result(ssl);
	
	if(VeryErr==X509_V_OK){
		MessageBoxA(NULL,"Verify_OK!",NULL,MB_OK);
	}else{
		sprintf(msg,"%d",VeryErr);

		MessageBoxA(NULL,msg,NULL,MB_OK);
	}
	*/
	WideToMultiChar(host,lstrlen(host),&m_host);
	return true;
}

bool OpenSSL::Request(const wchar_t* cmd,const wchar_t* path,const wchar_t* head,const wchar_t* content){
	string m_request,cmd_mb,path_mb,head_mb,content_mb;

	wstring w_request;
	//func.GetRequestString(cmd,TEXT("https"),m_host.c_str(),path,head,content,&w_request);

	//WideToMultiChar(w_request.c_str(),w_request.size()&m_request);
	SSL_write(ssl,(char*)m_request.c_str(),m_request.size());
	return true;
}

bool OpenSSL::Response(wstring *res){
	int total_readed=0;
	res->clear();
	while(1){
		char buf[100];
		int read_size;

		read_size=SSL_read(ssl,buf,sizeof(buf)-1);
		res->resize(total_readed+read_size);
		total_readed+=MultiByteToWideChar(CP_UTF8,NULL,buf,read_size,(wchar_t*)res->c_str()+total_readed,read_size);
		if(read_size>0){
			buf[read_size]=0;
		}else{
			break;
		}
	}
	return true;
}


bool OpenSSL::Async(ASYNC_CALLBACK callback){
	unsigned int res;
	call.callback=callback;
	call.pthis=this;
	hThread=(HANDLE)_beginthreadex(NULL,NULL,Async_Call,(LPVOID)&call,NULL,&res);
	return true;
}

unsigned int CALLBACK OpenSSL::Async_Call(LPVOID data){
	struct CALL* tmp=(CALL*)data;
	//return tmp->pthis->Async_Response(tmp->callback);
	return tmp->pthis->Async_Response(tmp);
}

unsigned int CALLBACK OpenSSL::Async_Response(LPVOID callback){
	wstring res;
	OpenSSL* pthis=((CALL*)callback)->pthis;
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
			Response(&res);
			((CALL*)callback)->callback((wchar_t*)res.c_str());
			break;
		}
	}
	WSACloseEvent(hEvent);
	_endthreadex(0);
	CloseHandle(hThread);

	return true;

	/*fd_set fds;
	FD_ZERO(&fds);
	FD_SET(m_socket.GetSocket(),&fds);*/
}

bool OpenSSL::Init(){
	ssl=NULL;
	ctx=NULL;
	return true;
}

bool OpenSSL::Reset(){
	if(ssl!=NULL){
		SSL_shutdown(ssl);
		SSL_free(ssl);
		ssl=NULL;
	}
	if(ctx!=NULL){
		SSL_CTX_free(ctx);
		ctx=NULL;
	}
	return true;
}
bool OpenSSL::End(){
	Reset();
	return true;
}
/*
void OpenSSL::Get(const wchar_t* cmd,const wchar_t* host,const wchar_t* path,const wchar_t *head,const wchar_t*content){
	Socket socket;
	int i,err,s;
	DWORD res;
	long VeryErr;

	char msg[1000],request[10000];
	char *str;
	SSL_CTX *ctx;
	SSL *ssl;
	X509 *server_cert;

	socket.Connect(host,TEXT("https"));

	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(socket.GetSocket(),&fds);
	

	//socket.Connect(TEXT("api.twitter.com"),TEXT("https"));

	SSL_library_init();
	SSL_load_error_strings();

	ctx=SSL_CTX_new(SSLv23_client_method());
	//i=SSL_CTX_load_verify_locations(ctx,NULL,"D:/MyDocuments/Visual Studio 2010/Projects/FILER/FILER/BuiltinObjectToken-VerisignClass3PublicPrimaryCertificationAuthority-G2.cer");
	//i=SSL_CTX_load_verify_locations(ctx,NULL,"./BuiltinObjectToken-VerisignClass3PublicPrimaryCertificationAuthority-G2.cer");
	SSL_CTX_set_verify_depth(ctx,SSL_VERIFY_PEER);

	ssl=SSL_new(ctx);
	SSL_set_fd(ssl,socket.GetSocket());
	err=SSL_connect(ssl);

	SSL_SESSION *ssi=SSL_get_session(ssl);

	sprintf(msg,"使用する暗号化方式=%s\n",SSL_get_cipher(ssl));
	MessageBoxA(NULL,msg,NULL,MB_OK);

	server_cert=SSL_get_peer_certificate(ssl);
	MessageBoxA(NULL,"Server_Certificate\n",NULL,MB_OK);

	str=X509_NAME_oneline(X509_get_subject_name(server_cert),NULL,NULL);

	sprintf(msg,"Subject:%s",str);
	MessageBoxA(NULL,msg,NULL,MB_OK);

	str=X509_NAME_oneline(X509_get_issuer_name(server_cert),NULL,NULL);

	sprintf(msg,"Issuer:%s",str);
	MessageBoxA(NULL,msg,NULL,MB_OK);

	VeryErr=SSL_get_verify_result(ssl);
	
	if(VeryErr==X509_V_OK){
		MessageBoxA(NULL,"Verify_OK!",NULL,MB_OK);
	}else{
		sprintf(msg,"%d",VeryErr);

		MessageBoxA(NULL,msg,NULL,MB_OK);
	}
	
	sprintf(request,"GET %s?%s HTTP/1.1\r\nHost:%s\r\nUser-Agent:Tw-Cl/2.0\r\n\r\n","/2/user.json",head,"userstream.twitter.com");

	err=SSL_write(ssl,request,strlen(request));
	//SSL_write(ssl,head,strlen(head));

	while(1){
		char buf[1000];
		int read_size;

		select(0,&fds,NULL,NULL,NULL);
		if(FD_ISSET(socket.GetSocket(),&fds)){
			read_size=SSL_read(ssl,buf,sizeof(buf)-1);
			buf[read_size]=0;
			MessageBoxA(NULL,buf,NULL,MB_OK);
			if(read_size>0){
			}else{
				//break;
			}
		}
	}

	SSL_shutdown(ssl);
	SSL_free(ssl);
	SSL_CTX_free(ctx);
}
*/
/*
LONG APIENTRY OpenSSL::MainProc(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp){
	switch(msg){
	case WM_WINSOCK:
		break;
	}

}
*/

Thread* Thread::Create(THREAD_CALL cb){
	Thread* p=new Thread();
	p->Start(cb);
	return p;
}
bool Thread::Start(THREAD_CALL cb){
	unsigned int res;
	hThread=(HANDLE)_beginthreadex(NULL,NULL,cb,(LPVOID)this,NULL,&res);
	return true;
}

void Thread::End(){
	delete this;
}



void Safe_Thread::Create(THREAD_CALL cb){
	call=cb;
	Thread::Create(Callback);
}

void Safe_Thread::End(){
	if(thread!=NULL){
		thread->End();
		thread=NULL;
	}
}

unsigned int CALLBACK Safe_Thread::Callback(void* cb){
	return ((Safe_Thread*)cb)->Func_Call(cb);
}


unsigned int CALLBACK Safe_Thread::Func_Call(void* cb){
	((THREAD_CALL)cb)(NULL);
	End();
	return 0;
}


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