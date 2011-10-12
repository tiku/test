#include "MAIN_VIEWER.h"

void Main_Viewer::Create(HWND hParent){
	HINSTANCE hInst=(HINSTANCE)GetWindowLongPtr(hParent,GWLP_HINSTANCE);
	LPCTSTR ClassName=TEXT("Main_Viewer");
	m_hWnd=AutoRegistCreateWnd(ClassName,WS_CHILD|WS_VISIBLE|WS_VSCROLL,0,0,400,400,hParent,NULL,hInst);

	memset(&m_VScrInfo,0,sizeof(m_VScrInfo));
	m_VScrInfo.cbSize=sizeof(m_VScrInfo);
	m_VScrInfo.fMask=SIF_PAGE|SIF_POS|SIF_RANGE|SIF_TRACKPOS|SIF_DISABLENOSCROLL;
	SetScrollInfo(m_hWnd,SB_VERT,&m_VScrInfo,FALSE);
}

LRESULT CALLBACK Main_Viewer::WindowProc(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp){
	switch(msg){
	case WM_CREATE:
		return 0;
	case WM_MOUSEMOVE:
		return 0;
	case WM_LBUTTONDOWN:

		return 0;
	case WM_LBUTTONUP:
		return 0;
	case WM_SIZE:
		MoveWindow(hWnd,0,0,400,400,TRUE);
		return 0;
	}
	return DefWindowProc(hWnd,msg,wp,lp);
}

