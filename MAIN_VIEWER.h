#ifndef MAIN_VIEWER
#define MAIN_VIEWER

#include "WINDOW.h"
#include <Windows.h>
#include <vector>
#include <map>
#include <algorithm>
#include <stack>

using namespace std;

struct draw_font_data{
	int begin,end;
	HFONT hFont;
	COLORREF back,text;
};
class area_data{
protected:
public:
	vector<pair<int,int>> m_area;
	HFONT m_hFont;
	COLORREF bk_color,text_color;
public:
	void Area_Set(int beg,int end);
	void Area_Not(int,int);
	void Area_Or(int,int);
	void Area_And(int,int);
	void SetFont(int,const wchar_t*,bool,bool,bool);
	void SetFontDirect(LOGFONT*);
	void SetColor(COLORREF,COLORREF);
	void Area_Sort(){
		int i,j;
		for(i=0;i<m_area.size()-1;i++){
			for(j=m_area.size();i<j;j--){
				if(m_area[j].first<m_area[j-1].first){
					swap(m_area[j],m_area[j-1]);
				}
			}
		}
	}
	void Draw(HDC);
};

class area_color:public area_data{
public:
	COLORREF m_back_color,m_text_color;
};

class area_font:public area_data{
	HFONT m_hFont;
};


class text_draw{
private:
public:
	map<wstring,area_data> areas;
	vector<draw_font_data> dfds;
	struct fc_data{
		HFONT hFont;
		COLORREF tx,bk;
	};
	wstring m_text;
public:
	void SetText(const wchar_t* text){
		m_text.assign(text);
	}
	void SetDFD(){
		map<wstring,area_data>::iterator itr;
		for(itr=areas.begin();itr!=areas.end();itr++){
			draw_font_data dfd;
			dfd.hFont=itr->second.m_hFont;
			dfd.back=itr->second.bk_color;
			dfd.text=itr->second.text_color;
			int i;
			for(i=0;i<itr->second.m_area.size();i++){
				dfd.begin=itr->second.m_area[i].first;
				dfd.end=itr->second.m_area[i].second;
				dfds.push_back(dfd);
			}
		}
	}
	void Sort(){
		int i,j;
		for(i=0;i<dfds.size()-1;i++){
			for(j=dfds.size()-1;i<j;j--){
				if(dfds[j].begin<dfds[j-1].begin){
					swap(dfds[j],dfds[j-1]);
				}
			}
		}
	}
	void Draw(HDC hdc){
		int i,width=0;
		wstring draw_text;
		SIZE tmp;
		stack<draw_font_data> dfd_stack;

		SetDFD();
		Sort();
		for(i=0;i<dfds.size()-1;i++){
			dfd_stack.push(dfds[i]);
			SelectObject(hdc,dfd_stack.top().hFont);

			draw_text.assign(m_text,dfds[i].begin,min(dfds[i].end-dfds[i].begin,dfds[i+1].begin-dfds[i].begin));
			TextOut(hdc,width,0,draw_text.c_str(),draw_text.size());
			GetTextExtentPoint32(hdc,draw_text.c_str(),draw_text.size(),&tmp);
			width+=tmp.cx;


			if(0<(dfds[i+1].begin-dfds[i].end)){
				dfd_stack.pop();
				//SetTextColor(hdc,dfd_stack.top().text);
				//SetBkColor(hdc,dfd_stack.top().back);
				SelectObject(hdc,dfd_stack.top().hFont);
				draw_text.assign(m_text,dfds[i].end,dfds[i+1].begin-dfds[i].end);
				TextOut(hdc,width,0,draw_text.c_str(),draw_text.size());
				GetTextExtentPoint32(hdc,draw_text.c_str(),draw_text.size(),&tmp);
				width+=tmp.cx;
			}
		}
	}

};


/*
class Text_Datas{
	vector<pair<pair<int,int>,HFONT>> hfonts;
	vector<pair<pair<int,int>,pair<COLORREF,COLORREF>>> colors;

	map<wstring,TextData> m_datas;
};
*/


class area_data_s{
private:
	map<wstring,area_data_s> datas;
public:
	void Draw(HDC hdc){}
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