#include "TWITTER_API.h"

void Twitter_Api::Followers(){
	PairDataArray pda;
	wstring res;
	oauth.SetHeader(TEXT("GET"),TEXT("http://api.twitter.com/1/statuses/friends_timeline.json"));
	oauth.GetData(&pda);
	int sc=inet.Auto(TEXT("GET"),TEXT("http://api.twitter.com/1/statuses/friends_timeline.json"),PairDataArray(),pda,&res);
	json_parser.Parse((wchar_t*)res.c_str());
	int a,b;
	a=0;

	return;
}

void Twitter_Api::Update(const wchar_t* text){
	//Oauth.GetHeader
}

void User_Stream::Start(){
	wchar_t* cmd,*url;
	PairDataArray pda;

	cmd=TEXT("GET");
	
	url=TEXT("https://userstream.twitter.com/2/user.json");
	//url=TEXT("http://twitter.com/");

	oauth.SetHeader(cmd,url);
	oauth.GetData(&pda);

	//oauth.GetHeader(cmd,url,&pda);
	//oauth.GetHeader(cmd,url,&pda);

	Inet inet;

	wa.Request(cmd,url,PairDataArray(),pda);
	wa.Response();
	//iac.Create(cmd,url,PairDataArray(),pda);
	//iac.Start();
	return;
	//inet.Connect(TEXT("userstream.twitter.com"),TEXT("https"));
	inet.Connect(TEXT("userstream.twitter.com"),TEXT("https"));
	inet.Request(cmd,url,PairDataArray(),pda);
	wstring a;
	inet.Response(&a);

	//iac.Create(cmd,url,PairDataArray(),pda);
	//Sleep(20000);
	//iac.Stop();
	//Sleep(10000);
	
	//inet_async.Request(cmd,url,PairDataArray(),pda);
	//inet_async.Start();
}

void User_Stream::Stop(){
	wa.Stop();
}
/*
unsigned int CALLBACK User_Stream::Callback(void* lpvoid){
	((User_Stream*)lpvoid)->Event_Function(NULL);
	return 0;
}

unsigned int CALLBACK User_Stream::Event_Function(void* arg){
	wstring res;
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
}

*/