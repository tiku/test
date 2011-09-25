#include "UNICODE.h"
//どうせ変換テーブル使うし
int WChar::WideToMultiChar(const wchar_t* src,int slen,string*dst){
	dst->resize(WideCharToMultiByte(CP_UTF8,NULL,src,slen,NULL,NULL,NULL,NULL));
	return WideCharToMultiByte(CP_UTF8,NULL,src,slen,(char*)dst->c_str(),dst->capacity(),NULL,NULL);
}

int WChar::MultiToWideChar(const char* src,int slen,wstring* dst){
	dst->resize(MultiByteToWideChar(CP_UTF8,NULL,src,slen,NULL,NULL));
	return MultiByteToWideChar(CP_UTF8,NULL,src,slen,(wchar_t*)dst->c_str(),dst->capacity());
}