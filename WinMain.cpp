#include "WINDOW.h"
#include "MAINWND.h"

int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrevInst,LPSTR pCmdline,int cmd){
	MSG msg;
	MainWindow MainWindow(hInst);
	while(GetMessage(&msg,NULL,0,0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
