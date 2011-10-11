#include "WINDOW.h"

Dialog*Dialog::s_pDialog=NULL;

ATOM Window::RegisterWC(HINSTANCE hInst,LPCTSTR Class){
	WNDCLASSEX wc;

	wc.cbSize=sizeof(WNDCLASSEX);
	wc.style=NULL;
	wc.lpfnWndProc=(WNDPROC)CallProc;
	wc.cbClsExtra=NULL;
	wc.cbWndExtra=NULL;
	wc.hInstance=hInst;
	wc.hIcon=(HICON)LoadImage(
		NULL,MAKEINTRESOURCE(IDI_APPLICATION),IMAGE_ICON,
		0,0,LR_DEFAULTSIZE|LR_SHARED);
	wc.hIconSm=wc.hIcon;
	wc.hCursor=(HCURSOR)LoadImage(
		NULL,MAKEINTRESOURCE(IDC_ARROW),IMAGE_CURSOR,
		0,0,LR_DEFAULTSIZE|LR_SHARED);
	wc.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName=NULL;
	wc.lpszClassName=Class;

	return RegisterClassEx(&wc);
}

HWND Window::CreateWnd(LPCTSTR Class,DWORD Style,int x,int y,int width,int height,HWND a_hWnd,HMENU hMenu,HINSTANCE hInst){
	HWND hWnd;
	RECT rc;
	hWnd=CreateWindow(
		Class,
		Class,
		Style,
		x,y,width,height,
		a_hWnd,
		hMenu,
		hInst,
		NULL
		);
	if(hWnd==NULL){
		MessageBox(NULL,TEXT("ウィンドウ作成失敗"),NULL,MB_OK);
	}
	SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG)this);

	GetWindowRect(hWnd,&rc);
	SendMessage(hWnd,WM_CREATE,NULL,NULL);
	SendMessage(hWnd,WM_SIZE,NULL,MAKELPARAM(rc.right-rc.left,rc.bottom-rc.top));
	SendMessage(hWnd,WM_SETFOCUS,NULL,NULL);
	return hWnd;
}

HWND Window::AutoRegistCreateWnd(LPCTSTR Class,DWORD Style,int x,int y,int width,int height,HWND hParent,HMENU hMenu,HINSTANCE hInst){
	RegisterWC(hInst,Class);
	return CreateWnd(Class,Style,x,y,width,height,hParent,hMenu,hInst);
}

LRESULT Window::CallProc(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp){
	Window* WndThis=(Window*)GetWindowLongPtr(hWnd,GWLP_USERDATA);
	if(WndThis){
		return WndThis->WindowProc(hWnd,msg,wp,lp);
	}
	return DefWindowProc(hWnd,msg,wp,lp);
}

void Dialog::Create(HWND hWnd,wchar_t* id){
	s_pDialog=this;
	DialogBoxParam((HINSTANCE)GetWindowLongPtr(hWnd,GWLP_HINSTANCE),id,NULL,CallProc,(LPARAM)this);
	//m_hWnd=CreateDialogParam((HINSTANCE)GetWindowLongPtr(hWnd,GWLP_HINSTANCE),id,hWnd,CallProc,(LPARAM)this);
}

BOOL Dialog::CallProc(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp){
	if(msg==WM_INITDIALOG){
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG)lp);
		//SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG)s_pDialog);
		s_pDialog=NULL;
	}
	Dialog* pDialog=(Dialog*)GetWindowLongPtr(hWnd,GWLP_USERDATA);
	if(pDialog==NULL){
		return 0;
	}
	return pDialog->DialogProc(hWnd,msg,wp,lp);
}

HWND DialogWindow::CreateDialogWindow(HWND hWnd,wchar_t* Class){
	HWND res;
	RECT rc;
	HINSTANCE hInst=(HINSTANCE)GetWindowLongPtr(hWnd,GWLP_HINSTANCE);
	m_hParent=hWnd;
	GetWindowRect(m_hParent,&rc);

	RegisterWC(hInst,Class);
	res=CreateWnd(Class,WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_VISIBLE|WS_MINIMIZEBOX,rc.left,rc.top,0,0,NULL,NULL,hInst);
	m_hDlg=CreateDialogParam(hInst,Class,res,(DLGPROC)DlgCallProc,(LPARAM)this);
	SendMessage(res,WM_SIZE,NULL,NULL);
	ShowWindow(m_hParent,SW_HIDE);
	return res;
}

void DialogWindow::CloseDialogWindow(){
	SendMessage(GetParent(m_hDlg),WM_CLOSE,NULL,NULL);
}


LRESULT CALLBACK DialogWindow::WindowProc(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp){
	RECT rc;
	RECT rrc;
	POINT pos;
	switch(msg){
	case WM_SIZE:
		GetWindowRect(m_hDlg,&rc);
		GetWindowRect(hWnd,&rrc);
		MoveWindow(hWnd,rrc.left,rrc.top,rc.right-rrc.left,rc.bottom-rrc.top,TRUE);
		return 0;
	case WM_DESTROY:
		DestroyWindow(m_hDlg);
		ShowWindow(m_hParent,SW_SHOW);
		return 0;
	}
	return DefWindowProc(hWnd,msg,wp,lp);
}

BOOL CALLBACK DialogWindow::DlgCallProc(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp){
	if(msg==WM_INITDIALOG){
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG)lp);
	}
	DialogWindow* pdw=(DialogWindow*)GetWindowLongPtr(hWnd,GWLP_USERDATA);
	if(pdw){
		pdw->DialogProc(hWnd,msg,wp,lp);
	}
	return false;
}

