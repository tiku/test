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
/*
void User_Stream::Start(){
	wchar_t* cmd,*url;
	PairDataArray pda;

	cmd=TEXT("GET");
	url=TEXT("http://api.twitter.com/1/statuses/friends_timeline.json");


	oauth.SetHeader(cmd,url);
	oauth.GetData(&pda);
	inet_async.Auto(cmd,url,PairDataArray(),pda,Callback,this);
}

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