#ifndef MAINWND
#define MAINWND
#include "WINDOW.h"
#include "JSON_PARSER.h"

class MainWindow:Window{
private:
public:
	HWND hWnd;
public:
	MainWindow(HINSTANCE);
	LRESULT CALLBACK WindowProc(HWND,UINT,WPARAM,LPARAM);

	static DWORD CALLBACK test(wchar_t* str){
		Json_Parser jp;
		jp.Parse(wcschr(str,'{'));
		int a;
		a=0;
		return a;
	};
};

#endif