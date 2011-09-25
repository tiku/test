#ifndef UNICODE_HEADER
#define UNICODE_HEADER

#include <Windows.h>
#include <string>

using namespace std;
namespace WChar{
	int WideToMultiChar(const wchar_t*,int,string*);
	int MultiToWideChar(const char*,int,wstring*);
}
#endif