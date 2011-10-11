#include "TWITTER_API.h"

void Twitter_Api::Followers(){
	PairDataArray pda;
	wchar_t*cmd,*url;
	Value json;
	wstring res;
	cmd=TEXT("GET");
	url=TEXT("http://api.twitter.com/1/statuses/friends_timeline.json");
	//oauth.SetHeader(cmd,url);
	//oauth.GetData(&pda);
	oauth.GetHeader(cmd,url,&pda);
	inet.Request(cmd,url,pda,PairDataArray());
	inet.Response(&res);
	json_parser.Parse((wchar_t*)res.c_str(),&json);
	Json_Arr arr;
	Json_Obj obj;
	json.Get_Arr(&arr);
	arr[0].Get_Obj(&obj);
	wstring test;
	obj[TEXT("user")].Get_Obj(&obj);
	obj[TEXT("name")].Get_ToStr(&test,NULL);
	MessageBox(NULL,test.c_str(),test.c_str(),MB_OK);
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
	wstring hd;
	wstring send;

	cmd=TEXT("GET");
	
	url=TEXT("https://userstream.twitter.com/2/user.json");
	//url=TEXT("http://twitter.com/");

	oauth.SetHeader(cmd,url);
	oauth.GetData(&pda);
	int i;
	for(i=0;i<pda.Size();i++){
		hd.append(pda[i].key).append(TEXT("=")).append(pda[i].data).append(TEXT("&"));
	}
	hd.erase(hd.size()-1);
	//oauth.GetHeader(cmd,url,&pda);
	//oauth.GetHeader(cmd,url,&pda);

	Inet inet;
	send.resize(1400);
	wsprintf((wchar_t*)send.c_str(),TEXT("%s %s?%s HTTP/1.1\r\nHost:userstream.twitter.com\r\n\r\n"),cmd,url,hd.c_str());
	wa.Connect(TEXT("userstream.twitter.com"),TEXT("https"));
	wa.Send(send.c_str(),send.size());
	wa.Recv(Recv_Callback);
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
	wa.EventStop();
}

void User_Stream::Restart(){
	wa.EventStart();
}

void User_Stream::End(){
	wa.EventEnd();
}

bool User_Stream::Recv_Callback(UINT msg,wchar_t* recv){
	MessageBox(NULL,recv,NULL,MB_OK);
	return false;
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