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



class User_Stream{
private:
	Safe_Thread *thread;
	Inet inet;
public:
	
};



#endif