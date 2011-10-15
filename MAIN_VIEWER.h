#ifndef MAIN_VIEWER
#define MAIN_VIEWER

#include "WINDOW.h"
#include <Windows.h>
#include <vector>
#include <map>

using namespace std;

class data_color{
public:
	int begin,end;
	COLORREF m_txcol,m_bkcol;
};

class data_font{
public:
	int begin,end;
	HFONT hFont;
};

class text_data{
protected:
	int m_begin,m_end;
	HFONT m_hFont;
	COLORREF m_bk_color,m_text_color;

	text_data* m_parent;
	map<wstring,text_data*> m_child;
public:
	text_data(){
		m_parent=NULL;
	}
	text_data(text_data* pp){
		m_parent=pp;
	}
	~text_data(){
		map<wstring,text_data*>::iterator itr;
		for(itr=m_child.begin();itr!=m_child.end();itr++){
			delete itr->second;
		}
		m_child.clear();
	}
	text_data &operator[](const wchar_t* index){
		return *m_child[index];
	}

	bool AddChild(const wchar_t* id_str){
		if(m_child.count(id_str)!=0){
			return false;
		}
		m_child.insert(pair<wstring,text_data*>(id_str,new text_data(this)));
		return true;
	}
	
	bool SetFont(int size,const wchar_t* name,bool italic,bool underline,bool strike){
		LOGFONT lf;
		memset(&lf,0,sizeof(LOGFONT));
		lf.lfHeight=size;
		lf.lfItalic=italic;
		lf.lfUnderline=underline;
		lf.lfStrikeOut=strike;
		
		lstrcpyn(lf.lfFaceName,name,sizeof(lf.lfFaceName)/sizeof(wchar_t));
		m_hFont=CreateFontIndirect(&lf);
		return true;
	}
	
	bool SetColor(COLORREF text,COLORREF bk){
		m_text_color=text;
		m_bk_color=bk;
		return true;
	}

	bool SetArea(int begin,int end){
		if(end<=begin){
			return false;
		}
		if(m_parent){
			if(begin<m_parent->m_begin||m_end<end){
				return false;
			}
			//重複チェック
			map<wstring,text_data*>::iterator itr;
			for(itr=m_parent->m_child.begin();itr!=m_parent->m_child.end();itr++){
				if(itr->second->m_begin<end&&begin<itr->second->m_end){
					return false;
				}
			}
		}
		m_begin=begin;
		m_end=end;
		return true;
	}

	HFONT GetFont(){
		return m_hFont;
	}
	int get_begin(){
		return m_begin;
	}
	int get_end(){
		return m_end;
	}
	text_data* Get_Child(int index);

};

class text_draw{
public:
	wstring m_text;
	text_data m_data;
	void Draw(HDC hdc){
		wstring draw_str;
		HFONT oldfont;
		text_data * p_data;
		oldfont=(HFONT)GetCurrentObject(hdc,OBJ_FONT);

		SelectObject(hdc,m_data.GetFont());
		delete m_data[TEXT("sss")];
		draw_str.assign(m_data.get_begin(),m_data.get_end());
		//DrawText(hdc,
	}
};


/*
class Text_Datas{
	vector<pair<pair<int,int>,HFONT>> hfonts;
	vector<pair<pair<int,int>,pair<COLORREF,COLORREF>>> colors;

	map<wstring,TextData> m_datas;
};
*/

struct area_data{
private:
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
	void Draw(HDC);
};

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