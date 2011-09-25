#include "MAINWND.h"
#include "SOCKET.h"
#include "OAUTH.h"
#include"XML_PARSER.h"
#include "TWITTER_API.h"
#include "JSON_PARSER.h"

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
	switch(msg){
	case WM_CREATE:
		return 0;
	case WM_COMMAND:
		switch(wp){
		case 401:
			{
				Json_Parser jp;
				jp.Parse(NULL);
				//OauthWindow ow;
				//ow.Create(hWnd);
			}
			break;
		case 402:
			{
				Twitter_Api api;
				api.Followers();
				/*wchar_t* host=TEXT("api.twitter.com");
				wchar_t* cmd=TEXT("GET");
				wchar_t* path=TEXT("/1/statuses/friends_timeline.json");
				wchar_t* url=TEXT("https://api.twitter.com/1/statuses/friends_timeline.json");*/
				wchar_t* host=TEXT("userstream.twitter.com");
				wchar_t* cmd=TEXT("GET");
				wchar_t* path=TEXT("/2/user.json");
				wchar_t* url=TEXT("https://userstream.twitter.com/2/user.json");
				static OpenSSL os;
				wstring cont;
				wchar_t tmp[1000]={0};
				char head[1000]={0};
				char con[1000]={0};
				Oauth oauth;
				wstring a,b,c;
				
				vector<PairData> pd(1);
				oauth.GetHeader(cmd,url,&pd.front());
				oauth.GetOauthJoin(&cont,'&');
				oauth.GetOauthJoin(&pd.front().data,'&');
				wsprintf(tmp,TEXT("%s:%s"),pd.front().key.c_str(),pd.front().data.c_str());
				WideCharToMultiByte(CP_UTF8,NULL,tmp,wcslen(tmp),head,1000,NULL,NULL);
				//WideCharToMultiByte(CP_UTF8,NULL,cont.c_str(),cont.size(),con,1000,NULL,NULL);
				os.Connect(host);
				os.Request(cmd,path,NULL,cont.c_str());
				os.Async(test);

				//os.Get(con);

				//pd.front().key=TEXT("Authorization");
				//pd.front().data=TEXT("OAuth bbbbbbbbbbbbbbb");
			//https.Send(cmd,host,path,pd,NULL_PAIRS);
			}
			break;
		}
	}
	return DefWindowProc(hWnd,msg,wp,lp);
}