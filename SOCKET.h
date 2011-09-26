#ifndef SOCKET_
#define SOCKET_


#include "WinMain.h"
#include "WINDOW.h"
#include "UNICODE.h"

#include <WS2tcpip.h>
//#include <WinInet.h>//HTTPSはwininetに頼らざるをえない。渋々…
#include <winhttp.h>//winhttpに頼らざるをえない…
#include <iostream>
#include <vector>
#include <string>
#include <process.h>

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>


using namespace std;
using namespace WChar;

#define WM_WINSOCK WM_APP+1

#define WS_VERSION 2
#define MAX_RECV 1000

#define NULL_PAIRS (vector<PairData>(0))


#define HTTP_RES_OK TEXT("HTTP/1.1 200 OK\r\n")


class Inet;

typedef bool(CALLBACK* ASYNC_CALLBACK)(Inet*);

struct PairData{
	wstring key;
	wstring data;
	PairData(){}
	PairData(const wchar_t* k,const wchar_t* d){
		key=k;
		data=d;
	}
	PairData Set(const wchar_t* k,const wchar_t* d){
		key=k;
		data=d;
		return *this;
	}
};

class PairDataArray{
private:
	static const int npos=-1;
	vector<PairData> m_pairs;
public:
	PairDataArray();

	PairData& operator[](const int);
	wchar_t* operator[](const wchar_t*);
	//PairDataArray& operator=(const wchar_t*);

	bool Set(const wchar_t*,const wchar_t*);
	void Sort();

	bool Insert(const wchar_t*,const wchar_t*);
	bool Erase(const wchar_t* key);
	int Find(const wchar_t*);

	int Size();
	void Clear();
};


class Socket{
private:
public:
	SOCKET m_sock;
	wstring m_host;
	bool Init();
	bool End();
	bool Reset();
public:
	Socket();
	~Socket();

	SOCKET GetSocket();
	wchar_t* GetHost();

	bool Connect(const wchar_t*,const wchar_t*);
	bool UrlSplit(wchar_t*,wstring*,wstring*);
	bool UrlEncode(wchar_t*,wstring*);
	int Send(const wchar_t*,int,int);
	int Recv(wstring*,int);
};

/*
class Http{
private:
	Socket m_socket;
	wstring m_host;
public:
	void GetHeadStr(vector<PairData>,wstring*);
	void GetContentStr(vector<PairData>,wstring*);
	bool UrlSplit(const wchar_t*,wstring*,wstring*,wstring*);
	bool UrlEncode(const wchar_t*,wstring*);
	bool Send(const wchar_t*,const wchar_t*,vector<PairData>,vector<PairData>);
	bool Https(const wchar_t*,const wchar_t*,const wchar_t*,vector<PairData>,vector<PairData>);
	bool Recv(wstring*,wstring*,wstring*);
};
*/

class Http_If{
	virtual bool Connect(const wchar_t*)=0;
	virtual bool Request(const wchar_t*,const wchar_t*,const wchar_t*,const wchar_t*)=0;
	virtual bool Response(wstring*)=0;

};

class Http_Bs_If{
public:
	const static wchar_t* protocol;
public:
	virtual bool Connect(const wchar_t*)=0;
	virtual bool Send(const wchar_t*,int)=0;
	virtual bool Recv(wstring*)=0;
	virtual wchar_t* GetProtocol()=0;
};

class Http_Bs:public Http_Bs_If{
private:
	Socket m_socket;
	wstring m_host;
public:
	bool Connect(const wchar_t*);
	bool Send(const wchar_t*,int);
	bool Recv(wstring*);
	wchar_t* GetProtocol();
	static wchar_t* Protocol();
};

class Https_Bs:public Http_Bs_If{

private:
	Socket m_socket;
	SSL_CTX* ctx;
	SSL *ssl;
	wstring m_host;
public:
	bool Connect(const wchar_t*);
	bool Send(const wchar_t*,int);
	bool Recv(wstring*);
	wchar_t*GetProtocol();
	static wchar_t* Protocol();
};

//HTTP補助関数群
//流用できそうなのでクラスに独立させる
class Http_Func{
public:
	bool GetRequestString(const wchar_t*,const wchar_t*,const wchar_t*,const wchar_t*,const wchar_t*,const wchar_t*,wstring*);
	bool GetRequestString(const wchar_t*,const wchar_t*,const wchar_t*,const wchar_t*,wstring*);
	bool UrlSplit(const wchar_t*,wstring*,wstring*,wstring*);
	bool UrlEncode(const wchar_t*,wstring*);
	void HeaderJoin(PairDataArray,wstring*);
	void ContentJoin(PairDataArray,wstring*);
};



class Inet{
private:
	Http_Bs_If* m_http;
	Http_Func m_func;
	wstring m_host;
	wstring m_response;
public:
	Inet();
//	Inet(ASYNC_CALLBACK);
	~Inet();

	void Init();
//	void Init(ASYNC_CALLBACK);
	void Reset();

	bool Connect(const wchar_t*,const wchar_t*);
	bool Async_Connect(const wchar_t*);
	bool Request(const wchar_t*,const wchar_t*,const wchar_t*,const wchar_t*);
	int Response(wstring*);

	bool StatusCodeAnalysis(wstring,int*);
	bool HeadAnalysis(const wchar_t*,PairDataArray*);
	bool ResponseSplit(const wchar_t*,wstring*,wstring*,wstring*);

	bool SetDefaultHeader(PairDataArray*);

	int Auto(const wchar_t*,const wchar_t*,PairDataArray,PairDataArray,wstring*);
};




class OpenSSL{
	typedef DWORD(CALLBACK* ASYNC_CALLBACK)(wchar_t*);
	Http_Func func;
	struct CALL{
		ASYNC_CALLBACK callback;
		OpenSSL* pthis;
	}call;
	class Socket m_socket;
	string m_host;
	SSL* ssl;
	SSL_CTX *ctx;
	HANDLE hThread;

public:
	bool Connect(const wchar_t*);
	bool Request(const wchar_t*,const wchar_t*,const wchar_t*,const wchar_t*);
	bool Response(wstring*);
	bool Async(ASYNC_CALLBACK);
	static unsigned int CALLBACK Async_Call(LPVOID);
	unsigned int CALLBACK Async_Response(LPVOID);
	bool Init();
	bool Reset();
	bool End();
	//void Get(const wchar_t*,const wchar_t*,const wchar_t*,const wchar_t*,const wchar_t*);
	void Send();
	void Recv();
};

class Http{
private:
	Http_Func func;
	Socket m_socket;
	wstring m_host;
public:
	void GetHeadStr(vector<PairData>,wstring*);
	void GetContentStr(vector<PairData>,wstring*);
	bool UrlSplit(const wchar_t*,wstring*,wstring*,wstring*);
	bool UrlEncode(const wchar_t*,wstring*);
	bool Send(const wchar_t*,const wchar_t*,vector<PairData>,vector<PairData>);
	bool Https(const wchar_t*,const wchar_t*,const wchar_t*,vector<PairData>,vector<PairData>);
	bool Recv(wstring*,wstring*,wstring*);

	bool Connect(const wchar_t*);
	bool Request(const wchar_t*,const wchar_t*,const wchar_t*,const wchar_t*);
	bool Response(wstring*);
};

//typedef unsigned(CALLBACK* THREAD_CALL)(void*);

class Thread{
//#define THREAD_CALL unsigned int CALLBACK
	typedef unsigned(CALLBACK* THREAD_CALL)(void*);
private:
	HANDLE hThread;
private:
	Thread(){};
	Thread(Thread&){}
	~Thread(){
		CloseHandle(hThread);
		_endthreadex(0);
	}

public:

	static Thread* Create(THREAD_CALL);
	bool Start(THREAD_CALL);
	void End();
};

class Safe_Thread{
	typedef unsigned(CALLBACK* THREAD_CALL)(void*);
private:
	Thread* thread;
	THREAD_CALL call;
public:
	Safe_Thread(){
		thread=NULL;
	}
	void Create(THREAD_CALL cb);
	void End();
	static unsigned int CALLBACK Callback(void* cb);
	unsigned int CALLBACK Func_Call(void* cb);
};

/*
class Async{
//#define THREAD_CALL unsigned int CALLBACK

private:
	ASYNC_CALLBACK call;
	Inet inet;
	HANDLE hThread;
	Async(){};
	~Async(){
		CloseHandle(hThread);
		_endthreadex(0);
	}
public:

	static void Create(ASYNC_CALLBACK acb){
		Async* p=new Async;
		p->Start(acb);
	}
	bool Start(ASYNC_CALLBACK);
	bool End();
	static THREAD_CALL Callback(void*);
	THREAD_CALL Async_func(void*);
};
*/
/*
class Async:private Async{
	Async(){
		Async* a=new Async();
		Async.Start();
	}
};
*/
#endif