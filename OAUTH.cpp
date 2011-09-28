#include "OAUTH.h"

Oauth::Oauth(){
	Init();
}

void Oauth::Init(){
	/*
	m_data.consumer_key=CONSUMER_KEY;
	m_data.consumer_secret=CONSUMER_SECRET;
	m_data.access_key=TEXT("");
	m_data.access_secret=TEXT("");

	m_oauth.clear();
	m_oauth.insert(map<wstring,wstring>::value_type(TEXT("oauth_consumer_key"),CONSUMER_KEY));
	m_oauth.insert(map<wstring,wstring>::value_type(TEXT("oauth_nonce"),TEXT("")));
	m_oauth.insert(map<wstring,wstring>::value_type(TEXT("oauth_signature_method"),TEXT("HMAC-SHA1")));
	m_oauth.insert(map<wstring,wstring>::value_type(TEXT("oauth_timestamp"),TEXT("")));
	m_oauth.insert(map<wstring,wstring>::value_type(TEXT("oauth_token"),TEXT("")));
	m_oauth.insert(map<wstring,wstring>::value_type(TEXT("oauth_version"),TEXT("1.0")));
	m_oauth.insert(map<wstring,wstring>::value_type(TEXT("oauth_verifier"),TEXT("")));
	*/
	
	/*
	m_data.consumer_key=CONSUMER_KEY;
	m_data.consumer_secret=CONSUMER_SECRET;
	m_data.access_key=TEXT("228108408-urFCbzrea5s6zepqLI1nJESk0Zxv7dTXZtFdWS0X");
	m_data.access_secret=TEXT("DLegqhz75xiQ5W8BRBqUgUUhbLwZ8tHxmweuRKLazg");

	m_oauth.clear();
	m_oauth.insert(map<wstring,wstring>::value_type(OAUTH_CONSUMER_KEY,CONSUMER_KEY));
	m_oauth.insert(map<wstring,wstring>::value_type(OAUTH_NONCE,TEXT("")));
	m_oauth.insert(map<wstring,wstring>::value_type(OAUTH_SIGNATURE_METHOD,TEXT("HMAC-SHA1")));
	m_oauth.insert(map<wstring,wstring>::value_type(OAUTH_TIMESTAMP,TEXT("")));
	m_oauth.insert(map<wstring,wstring>::value_type(OAUTH_TOKEN,m_data.access_key));
	m_oauth.insert(map<wstring,wstring>::value_type(OAUTH_VERSION,TEXT("1.0")));
	*/

	
	m_data.consumer_key=CONSUMER_KEY;
	m_data.consumer_secret=CONSUMER_SECRET;
	m_data.access_key=TEXT("228108408-urFCbzrea5s6zepqLI1nJESk0Zxv7dTXZtFdWS0X");
	m_data.access_secret=TEXT("DLegqhz75xiQ5W8BRBqUgUUhbLwZ8tHxmweuRKLazg");

	m_oauth.Insert(OAUTH_CONSUMER_KEY,CONSUMER_KEY);
	m_oauth.Insert(OAUTH_NONCE,TEXT(""));
	m_oauth.Insert(OAUTH_SIGNATURE_METHOD,TEXT("HMAC-SHA1"));
	m_oauth.Insert(OAUTH_TIMESTAMP,TEXT(""));
	m_oauth.Insert(OAUTH_TOKEN,m_data.access_key.c_str());
	m_oauth.Insert(OAUTH_VERSION,TEXT("1.0"));

	return;
}

bool Oauth::Step1(){
	Init();
	if(!GetRequestToken()){
		return false;
	}
	GetVerifier();
	return true;
}
bool Oauth::Step2(wchar_t* veri){
	SetVerifier(veri);
	/*
	
	PairData pd;
	GetHeader(&pd);
	wstring head;
	head.append(pd.key).append(TEXT(":")).append(pd.data);
	HINTERNET hSession,hConnect,hRequest;

	hSession=WinHttpOpen(TEXT("WinHttp /6.1"),
		WINHTTP_ACCESS_TYPE_NO_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS,NULL);
	hConnect=WinHttpConnect(hSession,TEXT("twitter.com"),INTERNET_DEFAULT_HTTPS_PORT,NULL);

	hRequest=WinHttpOpenRequest(
		hConnect,TEXT("POST"),
		TEXT("/oauth/access_token"),
		NULL,
		WINHTTP_NO_REFERER,
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		WINHTTP_FLAG_SECURE);

	WinHttpSendRequest(hRequest,head.c_str(),head.size(),WINHTTP_NO_REQUEST_DATA,NULL,NULL,NULL);

	WinHttpReceiveResponse(hRequest,NULL);


	DWORD dwStatusCode = 0;
	DWORD statusCodeSize = sizeof(DWORD);
 
	::WinHttpQueryHeaders(
    hRequest,
    WINHTTP_QUERY_STATUS_CODE |
    WINHTTP_QUERY_FLAG_NUMBER,
    WINHTTP_HEADER_NAME_BY_INDEX,
    &dwStatusCode,
    &statusCodeSize,
    WINHTTP_NO_HEADER_INDEX);

	char buf[50000];
	WinHttpReadData(hRequest,buf,50000,NULL);*/
	return GetAccessToken();
}

bool Oauth::Send(const wchar_t* cmd,const wchar_t* url,PairDataArray content,wstring* res){
	int status;
	PairDataArray pd;

	SetHeader(cmd,url);
	GetData(&pd);
	inet.Request(cmd,url,pd,PairDataArray());
	status=inet.Response(res);
	if(status!=200){
		return false;
	}
	return true;
}

bool Oauth::GetRequestToken(){
	vector<PairData> pd(1);
	wstring status,header,body;
	int begin,end;

	if(!SetAccessTokens(TEXT("POST"),TEXT("http://twitter.com/oauth/request_token"))){
		return false;
	}

	m_oauth.Set(OAUTH_TOKEN,m_data.access_key.c_str());
	//m_oauth.insert(map<wstring,wstring>::value_type("oauth_token",m_data.access_key));
	return true;
}

bool Oauth::GetVerifier(){
	wstring url;
	url=TEXT("http://twitter.com/oauth/authorize?oauth_token=");
	url.append(m_data.access_key);
	//わからないのでブラウザに丸投げ（調べてたら３日無駄にした！）
	ShellExecute(NULL,TEXT("open"),url.c_str(),NULL,NULL,NULL);
	return true;
}

bool Oauth::SetVerifier(wchar_t* veri){
	m_oauth.Set(OAUTH_VERIFIER,veri);
	return true;
}

bool Oauth::GetAccessToken(){
	return SetAccessTokens(TEXT("POST"),TEXT("http://twitter.com/oauth/access_token"));
}

void Oauth::GetHeader(const wchar_t* cmd,const wchar_t* url,PairDataArray* res){
	wstring key,data;

	SetTimeStamp();
	SetNonce();
	SetSignature(cmd,url);

	key=TEXT("Authorization");
	GetOauthJoin(&data,',');
	data.insert(0,TEXT("OAuth "));
	res->Insert(key.c_str(),data.c_str());
	return;
}

void Oauth::GetData(PairDataArray* res){
	m_oauth.Sort();
	*res=m_oauth;
}

void Oauth::SetHeader(const wchar_t* cmd,const wchar_t* url){
	Init();
	SetTimeStamp();
	SetNonce();
	SetSignature(cmd,url);
}


bool Oauth::SetAccessTokens(const wchar_t* cmd,const wchar_t* url){
	wstring body;
	int begin,end;
	/*vector<PairData> pd(1);
	int begin,end;

	GetHeader(cmd,url,&pd[0]);
	if(!Send(cmd,url,pd,&body)){
		return false;
	}*/

	if(!Send(cmd,url,PairDataArray(),&body)){
		return false;
	}

	//帰ってきたデータをセットする
	begin=body.find('=',0)+strlen("=");
	end=body.find('&',begin);
	m_data.access_key.assign(body,begin,end-begin);
	begin=body.find('=',end)+strlen("=");
	end=body.find('&',begin);
	m_data.access_secret.assign(body,begin,end-begin);

	return true;
}


void Oauth::SetSignature(const wchar_t* cmd,const wchar_t* url){
	wstring key,msg,tmp;
	string mkey,mmsg;
	wstring signature;
	key.append(m_data.consumer_secret);
	key.append(TEXT("&"));
	key.append(m_data.access_secret);

	m_http_func.UrlEncode(cmd,&tmp);
	msg.append(tmp).append(TEXT("&"));
	m_http_func.UrlEncode(url,&tmp);
	msg.append(tmp).append(TEXT("&"));
	GetOauthJoin(&tmp,'&');
	m_http_func.UrlEncode(tmp.c_str(),&tmp);
	msg.append(tmp);

	HMac_Sha1 hms1;
	string msignature;
	WideToMultiChar(key.c_str(),key.size(),&mkey);
	WideToMultiChar(msg.c_str(),msg.size(),&mmsg);
	hms1.Encode((char*)mkey.c_str(),(char*)mmsg.c_str(),&msignature);
	MultiToWideChar(msignature.c_str(),msignature.size(),&signature);
	m_http_func.UrlEncode(signature.c_str(),&signature);

	//m_oauth["oauth_signature"]=signature;
	m_oauth.Insert(OAUTH_SIGNATURE,signature.c_str());
	return;
}

void Oauth::SetTimeStamp(){
	time_t timestamp;
	wchar_t tmp[64];
	time(&timestamp);
	wsprintf(tmp,TEXT("%I64d"),timestamp);
	m_oauth.Set(OAUTH_TIMESTAMP,tmp);
	return;
}

void Oauth::SetNonce(){
	srand(unsigned int(time));
	wchar_t tmp[64];
	wsprintf(tmp,TEXT("%I64d"),(unsigned long long)(rand()*rand()));
	m_oauth.Set(OAUTH_NONCE,tmp);
	return;
}

void Oauth::GetOauthJoin(wstring*res,wchar_t dl){
	int i;
	res->clear();
	m_oauth.Sort();
	for(i=0;i<m_oauth.Size();i++){
		res->append(m_oauth[i].key);
		res->append(TEXT("="));
		res->append(m_oauth[i].data);
		res->append(1,dl);
	}
	res->erase(res->size()-1);
}

void OauthWindow::Create(HWND hWnd){
	m_hWnd=CreateDialogWindow(hWnd,TEXT("OAUTH"));
}

BOOL CALLBACK OauthWindow::DialogProc(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp){
	switch(msg){
	case WM_INITDIALOG:
		if(!oauth.Step1()){
			MessageBox(hWnd,TEXT("リクエストトークンの取得に失敗しました"),NULL,MB_OK);
			CloseDialogWindow();
		}
		return true;
	case WM_COMMAND:
		switch(LOWORD(wp)){
		case IDC_AUTH:
			GetWindowText(GetDlgItem(hWnd,IDC_AUTH_EDIT),verifier,8);
			if(oauth.Step2(verifier)){
				MessageBox(hWnd,TEXT("認証しました"),TEXT("OK"),MB_OK);
				/*Https https;
				vector<PairData> pd(1);
				oauth.m_oauth.erase(oauth.m_oauth.find(TEXT("oauth_verifier")));
				oauth.GetHeader(&pd.front());
				https.Send(TEXT("GET"),TEXT("twitter.com"),TEXT("oauth/authorize"),NULL_PAIRS,NULL_PAIRS);
				//https.Send(TEXT("GET"),TEXT("userstream.twitter.com"),TEXT("/2/user.json"),pd,NULL_PAIRS);*/
			}else{
				MessageBox(hWnd,TEXT("アクセストークンの取得に失敗しました"),NULL,MB_OK);
			}
			CloseDialogWindow();
			break;
		}
		return true;
	}
	return false;
}