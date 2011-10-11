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

typedef bool(CALLBACK* ASYNC_CALLBACK)(UINT,wchar_t*);

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

class Socket_Base{
protected:
	SOCKET m_sock;
	wstring m_host;

	bool Base_Connect(const wchar_t*,const wchar_t*);
	void Base_Reset();
public:
	Socket_Base();
	virtual ~Socket_Base();

	virtual void Reset();
	virtual bool Connect(const wchar_t*);
	virtual int Send(const wchar_t*,int);
	virtual int Recv(wstring*);
	
	SOCKET GetSocket();
};


class SSL_Socket:public Socket_Base{
private:
	SSL *ssl;
	SSL_CTX *ctx;
public:
	SSL_Socket();
	~SSL_Socket();

	void Reset();
	bool Connect(const wchar_t*);
	int Send(const wchar_t*,int);
	int Recv(wstring*);
};


class Socket{
private:
	Socket_Base* m_socket;
public:
	Socket();
	~Socket();

	void Reset();

	bool Connect(const wchar_t*,const wchar_t*);
	int Send(const wchar_t*,int);
	int Recv(wstring*);
};


class WSA_Async{
private:
	HANDLE m_hThread;
	HANDLE m_hWSAEvent;
	HANDLE m_hEvent;
	bool pause;
	bool running;

	Socket_Base* m_socket;
	ASYNC_CALLBACK m_call;
	
	void ThreadStart(ASYNC_CALLBACK);

	void Event_Proc();
	
	static unsigned int CALLBACK CallProc(void*);
	unsigned int CALLBACK ThreadProc(void*);
public:
	WSA_Async();
	~WSA_Async();

	void Reset();
	bool Connect(const wchar_t*,const wchar_t*);
	int Send(const wchar_t*,int);
	int Recv(ASYNC_CALLBACK);

	void EventStart();
	void EventStop();
	void EventEnd();
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

	bool StatusCodeAnalysis(wstring,int*);
	bool HeadAnalysis(const wchar_t*,PairDataArray*);
	bool ResponseSplit(const wchar_t*,wstring*,wstring*,wstring*);

};



class Inet{
private:
	Socket m_socket;
	Http_Func m_func;
	wstring m_host;
	wstring m_response;
public:
	Inet();
	~Inet();

	void Reset();

	bool Connect(const wchar_t*,const wchar_t*);
	bool Async_Connect(const wchar_t*);
	bool Request(const wchar_t*,const wchar_t*,PairDataArray,PairDataArray);
	int Response(wstring*);
	bool Response_Async(ASYNC_CALLBACK);

	bool StatusCodeAnalysis(wstring,int*);
	bool HeadAnalysis(const wchar_t*,PairDataArray*);
	bool ResponseSplit(const wchar_t*,wstring*,wstring*,wstring*);

	bool SetDefaultHeader(PairDataArray*);

	int Auto(const wchar_t*,const wchar_t*,PairDataArray,PairDataArray,wstring*);
};



//typedef unsigned(CALLBACK* THREAD_CALL)(void*);

/*
class Inet_Async{
private:
	bool running;
	Socket m_socket;
	HANDLE hThread;
	Inet inet;

	void* arg;
	unsigned int CALLBACK call(void*);
public:
	void Auto(const wchar_t*,const wchar_t*,PairDataArray,PairDataArray,unsigned int CALLBACK(void*),void*);
	void Create(unsigned int CALLBACK(void*),void*);
	static unsigned int CALLBACK CallProc(void*);
	unsigned int CALLBACK ThreadProc(void*);

};
*/
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