#ifndef WINDOW
#define WINDOW
#include "WinMain.h"

#include "resource.h"

class Window{
protected:
	void GetAutoWC(WNDCLASSEX*);
public:
	Window(){}
	virtual ~Window(){}
	static ATOM RegisterWC(HINSTANCE,LPCTSTR,int=NULL);
	HWND CreateWnd(LPCTSTR,DWORD,int,int,int,int,HWND,HMENU,HINSTANCE);

	static LRESULT CALLBACK CallProc(HWND,UINT,WPARAM,LPARAM);
	virtual LRESULT CALLBACK WindowProc(HWND,UINT,WPARAM,LPARAM)=0;
};

class Dialog{
	HWND m_hWnd;
	static Dialog*s_pDialog;
public:
	void Create(HWND,wchar_t*);
	static BOOL CALLBACK CallProc(HWND,UINT,WPARAM,LPARAM);
	virtual BOOL CALLBACK DialogProc(HWND,UINT,WPARAM,LPARAM)=0;
};

class DialogWindow:private Window{
private:
	HWND m_hDlg;//ダイアログ
	HWND m_hParent;//親ウィンドウ
protected:
	HWND CreateDialogWindow(HWND,wchar_t*);
	void CloseDialogWindow();
	LRESULT CALLBACK WindowProc(HWND,UINT,WPARAM,LPARAM);
	static BOOL CALLBACK DlgCallProc(HWND,UINT,WPARAM,LPARAM);
	virtual BOOL CALLBACK DialogProc(HWND,UINT,WPARAM,LPARAM)=0;
};
#endif