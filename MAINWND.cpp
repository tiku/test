#include "MAINWND.h"
#include "SOCKET.h"
#include "OAUTH.h"
#include"XML_PARSER.h"
#include "TWITTER_API.h"
#include "JSON_PARSER.h"
#include "MAIN_VIEWER.h"

MainWindow::MainWindow(HINSTANCE hInst){
	wchar_t *name=TEXT("MAIN");
	RegisterWC(hInst,name);
	SetClassLongPtr(hWnd,GCLP_MENUNAME,(LONG)TEXT("MAIN"));
	SetClassLongPtr(hWnd,GCLP_HICON,(LONG)TEXT("MAIN_ICON"));
	hWnd=CreateWnd(
		name,WS_OVERLAPPEDWINDOW|WS_VISIBLE,
		CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
		NULL,LoadMenu(hInst,TEXT("MAIN")),hInst
		);
}

LRESULT CALLBACK MainWindow::WindowProc(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp){
	//static OauthWindow ow;
	static User_Stream us;
	switch(msg){
	case WM_CREATE:
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_COMMAND:
		switch(wp){
		case 401:
			{
				us.Start();
				//us.Stop();
				//us.Start2();
				//Json_Parser jp;
				//jp.Parse(NULL);
				//OauthWindow ow;
				//ow.Create(hWnd);
			}
			break;
		case 402:
			{
				static Main_Viewer a;
				a.Create(hWnd);
				
				break;;
				/*wchar_t* host=TEXT("api.twitter.com");
				wchar_t* cmd=TEXT("GET");
				wchar_t* path=TEXT("/1/statuses/friends_timeline.json");
				wchar_t* url=TEXT("https://api.twitter.com/1/statuses/friends_timeline.json");*/
				wchar_t* host=TEXT("userstream.twitter.com");
				wchar_t* cmd=TEXT("GET");
				wchar_t* path=TEXT("/2/user.json");
				wchar_t* url=TEXT("https://userstream.twitter.com/2/user.json");
				
			}
			break;
		case 403:
			us.Restart();
			break;
		case 404:
			us.End();
			break;
		case 405:
			{
				Twitter_Api api;
				api.Followers();
			}
		}
	}
	return DefWindowProc(hWnd,msg,wp,lp);
}