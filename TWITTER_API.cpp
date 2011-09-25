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