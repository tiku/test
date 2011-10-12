#ifndef MAIN_VIEWER
#define MAIN_VIEWER

#include "WINDOW.h"
#include <Windows.h>
#include <vector>

using namespace std;

struct DrawTemplate{
	RECT rc;
	vector<wstring> texts;
	vector<HANDLE> images;
};

class DrawTextData{
	
};

class Options{
	SCROLLINFO m_VScrInfo,m_HScrInfo;

};

class Main_Viewer:Window{
private:
	enum{
		ID_VSCROLL=1000,
		ID_HSCROLL
	};
	HWND m_hWnd,m_hParent;
	HWND m_hHScroll,m_hVScroll;
	SCROLLINFO m_HScrInfo,m_VScrInfo;
public:
	void Create(HWND);
public:
	void AddText(DrawTextData);
	LRESULT CALLBACK WindowProc(HWND,UINT,WPARAM,LPARAM);
};


#endif