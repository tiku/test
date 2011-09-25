#ifndef OAUTH_
#define OAUTH_
#include "WinMain.h"
#include "SOCKET.h"
#include "HMAC_SHA1.h"
#include "WINDOW.h"
#include "UNICODE.h"

using namespace WChar;

#include <string>
#include <ctime>
#include <map>

#define CONSUMER_KEY TEXT("r1yk7qZXEGYcaDgoBaI4NQ")
#define CONSUMER_SECRET TEXT("c4ydTrQU1M73cmr3tLFyQtjedFWHdfvsBgSdW52JQpY")

#define OAUTH_CONSUMER_KEY		TEXT("oauth_consumer_key")
#define OAUTH_NONCE				TEXT("oauth_nonce")
#define OAUTH_SIGNATURE			TEXT("oauth_signature")
#define OAUTH_SIGNATURE_METHOD	TEXT("oauth_signature_method")
#define OAUTH_TIMESTAMP			TEXT("oauth_timestamp")
#define OAUTH_TOKEN				TEXT("oauth_token")
#define OAUTH_VERIFIER			TEXT("oauth_verifier")
#define OAUTH_VERSION			TEXT("oauth_version")


//‚²‚¿‚á‚²‚¿‚á‚µ‚½‚¯‚Ç‚Ü‚ ‚¢‚¢‚â

class Oauth{
	Http Http;
	struct oauth_data{
		wstring consumer_key;
		wstring consumer_secret;
		wstring access_key;
		wstring access_secret;
	};
	struct request{
		wchar_t* command;
		wstring url;
	};
public:
	//map<wstring,wstring>m_oauth;
	PairDataArray m_oauth;
	oauth_data m_data;
public:
	Oauth();
	bool Step1();
	bool Step2(wchar_t*);
	bool Send(const wchar_t*,const wchar_t*,vector<PairData>,wstring*);

	void GetHeader(const wchar_t*,const wchar_t*,PairData*);
	void GetData(PairDataArray*);
	void SetHeader(const wchar_t*,const wchar_t*);
private:
	bool GetRequestToken();
	bool GetVerifier();
	bool SetVerifier(wchar_t*);
	bool GetAccessToken();
private:
	void Init();
	void SetOauth();
	void SetOauthHead();
	void SetSignature(const wchar_t*,const wchar_t*);
	void SetTimeStamp();
	void SetNonce();
public:
	void GetOauthJoin(wstring*,wchar_t);
private:
	bool SetAccessTokens(const wchar_t*,const wchar_t*);
};

class OauthWindow:public DialogWindow{
private:
	HWND m_hWnd;
	Oauth oauth;
	wchar_t verifier[16];
public:
	void Create(HWND);
	BOOL CALLBACK DialogProc(HWND,UINT,WPARAM,LPARAM);
};

#endif