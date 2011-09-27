#ifndef TWITTER_API
#define TWITTER_API

#include "OAUTH.h"
#include "XML_PARSER.h"
#include "JSON_PARSER.h"


class Twitter_Api{
	Json_Parser json_parser;
	Oauth oauth;
	Inet inet;
public:
	void Followers();
	void Update(const wchar_t* text);

	void Timeline();
	void User_Stream();
};


/*
class User_Stream{
private:
	Inet_Async inet_async;
	Oauth oauth;
public:
	void Start();
	static unsigned int CALLBACK Callback(void*);
	unsigned CALLBACK Event_Function(void*);

};
*/


#endif