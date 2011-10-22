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

class area_data_gui{
private:
	struct area_gui{
		int begin,end;
		vector<RECT> rc;
	};
public:
	area_gui area;
	draw_font_data font_data;
};

class area_back_color:public area_data{
public:
	COLORREF m_back_color;
};

class area_text_color:public area_data{
public:
	COLORREF m_text_color;
};

class area_font:public area_data{
public:
	HFONT m_hFont;
};

class area_all:public area_data{
public:
	HFONT m_hFont;
	COLORREF m_back_color,m_text_color;
};


class text_draw{
private:
public:
	struct area_gui_base{
		int begin,end;
		RECT rc;//ï°êîçsÇ…ìnÇÈÇ∆Ç´Ç∆Ç©Ç…Åc;
	};
	struct area_gui{
		area_gui_base area;
		HFONT hFont;
	};
	RECT m_rc;
	int m_begin,m_end;
	vector<area_gui> area_and_font;
	wstring m_text;
public:
	void Sort(){
		int i,j;
		for(i=0;i<area_and_font.size()-1;i++){
			for(j=area_and_font.size()-1;i<j;j--){
				if(area_and_font[j].area.begin<area_and_font[j-1].area.begin){
					swap(area_and_font[j],area_and_font[j-1]);
				}
			}
		}
	}
	void GetPointColumn(int x,int y){
		/*int i,j;
		for(i=0;i<area_and_font.size();i++){
			for(j=0;j<area_and_font[i].first.rc.size();j++){
				area_and_font[i].first.rc[j].y
				*/
	}
	void saiki(HDC hdc,int i,stack<area_gui*> hfonts){
		/*
		SIZE size;
		RECT rc;
		if(i==area_and_font.size()){
			return;
		}
		if(area_and_font[i].area.end<area_and_font[i+1].area.begin){
			SelectObject(hdc,area_and_font[i].hFont);
			GetTextExtentPoint32(hdc,m_text.c_str()+area_and_font[i].area.begin,area_and_font[i].area.end-area_and_font[i].area.begin,&size);
			rc.top=0;
			rc.left=hfonts.empty()?0:hfonts.top()->area.rc.right;
			rc.bottom=rc.top+size.cy;
			rc.right=rc.left+size.cx;
			hfonts.push(&area_and_font[i]);
			saiki(hdc,i+1,
		}else{
			SelectObject(
			saiki(hdc,*/
	}
	void SetRect(){
		SIZE tmp,size;

		RECT rc={0,0,0,0};
		stack<area_gui*> hfonts;
		area_gui prev;
		int prev_x=0,line;
		int beg,end;
		HDC hdc;


		Sort();

		hdc=GetDC(NULL);
		int i;
		for(i=0;i<area_and_font.size();i++){
			SelectObject(hdc,area_and_font[i].hFont);
			beg=area_and_font[i].area.begin;
			end=min(area_and_font[i].area.end,area_and_font.size()==(i+1)?area_and_font[i].area.end:area_and_font[i+1].area.begin);
			GetTextExtentPoint32(hdc,m_text.c_str()+beg,end-beg,&size);
			rc.left=prev_x;
			rc.top=0;
			rc.right=rc.left+size.cx;
			rc.bottom=rc.top+size.cy;
			area_and_font[i].area.rc=rc;
			prev_x=rc.right;
			hfonts.push(&area_and_font[i]);
			if(end==area_and_font[i].area.end){
				while(1){
					prev=*hfonts.top();
					hfonts.pop();
					if(hfonts.empty()||(i+1)==area_and_font.size()){
						break;
					}
					SelectObject(hdc,hfonts.top()->hFont);
					beg=prev.area.end;
					end=min(hfonts.top()->area.end,area_and_font.size()==(i+1)?hfonts.top()->area.end:area_and_font[i+1].area.begin);
					GetTextExtentPoint32(hdc,m_text.c_str()+beg,end-beg,&size);
					rc.left=hfonts.top()->area.rc.left;
					rc.top=0;
					rc.right=prev_x+size.cx;
					rc.bottom=rc.top+size.cy;
					prev_x=rc.right;
					hfonts.top()->area.rc=rc;
				}
			}
		}
		/*for(i=0;i<area_and_font.size();i++){
			SelectObject(hdc,area_and_font[i].hFont);
			beg=area_and_font[i].area.begin;
			end=area_and_font[i].area.end;
			GetTextExtentPoint32(hdc,m_text.c_str()+beg,end-beg,&tmp);

			rc.left=prev_x;
			rc.right=rc.left+tmp.cx;
			rc.top=0;
			rc.bottom=rc.top+tmp.cy;
			area_and_font[i].area.rc=rc;
			while(area_and_font[i].end*/
		/*
		for(i=0;i<area_and_font.size();i++){
			SelectObject(hdc,area_and_font[i].hFont);
			beg=area_and_font[i].area.begin;
			end=area_and_font[i].area.end;
			GetTextExtentPoint32(hdc,m_text.c_str()+beg,end-beg,&tmp);

			rc.left=prev_x;
			rc.right=rc.left+tmp.cx;
			rc.top=0;
			rc.bottom=rc.top+tmp.cy;
			area_and_font[i].area.rc=rc;
		
			if(i!=area_and_font.size()-1){
				if(area_and_font[i+1].area.begin<area_and_font[i].area.end){
					beg=area_and_font[i].area.begin;
					end=area_and_font[i+1].area.begin;
					GetTextExtentPoint32(hdc,m_text.c_str()+beg,end-beg,&tmp);
					prev_x+=tmp.cx;
					hfonts.push(area_and_font[i]);
				}else if(!hfonts.empty()){
					hfonts.push(area_and_font[i]);
					//while(hfonts.top().area.end<area_and_font[min(i+1,area_and_font.size())].area.begin){
					while(1){
						prev=hfonts.top();
						hfonts.pop();
						if(!hfonts.empty()&&hfonts.top().area.end<area_and_font[min(i+1,area_and_font.size())].area.begin){
						//if(!hfonts.empty()){
							SelectObject(hdc,hfonts.top().hFont);
							beg=prev.area.end;
							end=hfonts.top().area.end;
							GetTextExtentPoint32(hdc,m_text.c_str()+beg,end-beg,&tmp);
							prev_x+=tmp.cx;
						}else{
							if(i<area_and_font.size()&&!hfonts.empty()){
								SelectObject(hdc,hfonts.top().hFont);
								beg=prev.area.end;
								end=area_and_font[i+1].area.begin;
								GetTextExtentPoint32(hdc,m_text.c_str()+beg,end-beg,&tmp);
								prev_x+=tmp.cx;
							}
							break;
						}
					}
				}
			}
		}*/
		ReleaseDC(NULL,hdc);
		return;
	}
	void Add_Area(int begin,int end){
		area_gui area;
		area.area.begin=begin;
		area.area.end=end;
		Sort();
	}
	void SetText(const wchar_t* text){
		m_text.assign(text);
		m_begin=0;
		m_end=m_text.size();
	}
	void Draw(HDC hdc){
		SetRect();
		stack<area_gui> hfonts;
		area_gui prev;
		int i,beg,size;
		for(i=0;i<area_and_font.size();i++){
			if(i!=area_and_font.size()-1&&area_and_font[min(i+1,area_and_font.size())].area.begin<area_and_font[i].area.end){
					SelectObject(hdc,(HFONT)area_and_font[i].hFont);
					beg=area_and_font[i].area.begin;
					size=area_and_font[i+1].area.begin-beg;
					TextOut(hdc,area_and_font[i].area.rc.left,area_and_font[i].area.rc.top,m_text.c_str()+beg,size);
					hfonts.push(area_and_font[i]);
			}else{
					SelectObject(hdc,(HFONT)area_and_font[i].hFont);
					beg=area_and_font[i].area.begin;
					size=area_and_font[i].area.end-beg;
					TextOut(hdc,area_and_font[i].area.rc.left,area_and_font[i].area.rc.top,m_text.c_str()+beg,size);
					hfonts.push(area_and_font[i]);

					while(i!=area_and_font.size()-1&&2<=hfonts.size()){
					prev=hfonts.top();
					hfonts.pop();
					if(hfonts.top().area.end<area_and_font[min(i+1,area_and_font.size())].area.begin||i==area_and_font.size()-1){
						beg=prev.area.end;
						size=hfonts.top().area.end-beg;
						SelectObject(hdc,hfonts.top().hFont);
						TextOut(hdc,prev.area.rc.right,hfonts.top().area.rc.top,m_text.c_str()+beg,size);
					}else{
						SelectObject(hdc,hfonts.top().hFont);
						beg=prev.area.end;
						size=area_and_font[i+1].area.begin-beg;
						TextOut(hdc,prev.area.rc.right,hfonts.top().area.rc.top,m_text.c_str()+beg,size);
						break;
					}
					}
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