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

};

#endif