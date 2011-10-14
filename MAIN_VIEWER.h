#ifndef MAIN_VIEWER
#define MAIN_VIEWER

#include "WINDOW.h"
#include <Windows.h>
#include <vector>
#include <map>

using namespace std;

class TextData{
	wchar_t* m_text;
	int begin,end;
	COLORREF m_txcol,m_bkcol;
	HFONT m_hFont;
};


struct area_data{
private:
public:
	vector<pair<int,int>> areas;
	HFONT hFont;
	COLORREF bk_color,text_color;
public:
	void Area_Set(int beg,int end);
	void Area_Not(int,int);
	void Area_Or(int,int);
	void Area_And(int,int);
	void Area_Xor(int,int);
	void SetFont(int,const wchar_t*,bool,bool,bool);
	void SetColor(int,int);
};


class DrawTextData{
private:
	RECT m_rc;
	map<wstring,area_data> m_areas;

	void Reset();
	HFONT hFont,m_hFont;
	wstring m_text;
public:
	DrawTextData();
	~DrawTextData();
	void SetFontDirect(LOGFONT*);
	void SetFont(int,const wchar_t*,bool,bool,bool);
	void SetText(const wchar_t*);
	void SetArea(int,int,int,int);
	void Set(const wchar_t*,LOGFONT*,RECT*,COLORREF,COLORREF);

	void SetNewArea(int,int,const wchar_t*);
	void Area_Not(int,int,const wchar_t*);
	void Area_Or(int,int,const wchar_t*);
	void Area_And(int,int,const wchar_t*);

	void GetLG(POINT*);
	void Draw(HDC);
	//void Get
};


struct DrawTemplate{
	RECT m_rc;
	HFONT hFont;
	wstring m_text;

	vector<class DrawTextData> dtds;
	class DrawTextData m_textdata;

	void Set(const wchar_t* text);
	void Set(int,int,int,int);
	void Draw(HWND);

	void MouseMove(HWND,WPARAM,LPARAM);
	void LButtonDown(HWND,WPARAM,LPARAM);
	void RButtonDown(HWND,WPARAM,LPARAM);


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
	SCROLLINFO m_HScrInfo,m_VScrInfo;
public:
	void Create(HWND);
public:
	void AddText(DrawTextData);
	LRESULT CALLBACK WindowProc(HWND,UINT,WPARAM,LPARAM);
};


#endif