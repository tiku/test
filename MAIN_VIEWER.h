#ifndef MAIN_VIEWER
#define MAIN_VIEWER

#include "WINDOW.h"
#include <Windows.h>
#include <vector>
#include <map>

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
	void Draw(HDC);
};

class area_root:public area_data{
	wstring m_text;
};

class data:public area_data{
private:
	map<wstring,data*> m_child;
public:
	~data(){
		map<wstring,data*>::iterator itr;
		for(itr=m_child.begin();itr!=m_child.end();itr++){
			delete itr->second;
		}
	}
	data& operator [](const wchar_t* index){
		return *m_child[index];
	}
	bool AddChild(const wchar_t* id_str){
		if(m_child.count(id_str)!=0){
			return false;
		}
		m_child.insert(pair<wstring,data*>(id_str,new data()));
		return true;
	}
	void GetDrawData(vector<draw_font_data> res){
		map<wstring,data*>::iterator itr;
		for(itr=m_child.begin();itr!=m_child.end();itr++){
			
		}
	}

};




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

struct text_data_struct{
	int begin,end;
	HFONT hFont;
	COLORREF m_bk,color,m_text_color;
	text_data_struct* parent;
	map<wstring,text_data_struct*>child;
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
	text_data(text_data& ref){
		*this=ref;
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
	void Get_Area_Datas(vector<draw_font_data>* res){
		draw_font_data dfd;

		res->push_back(dfd);
	}
	/*
	HFONT GetFont(){
		return m_hFont;
	}
	int get_begin(){
		return m_begin;
	}
	int get_end(){
		return m_end;
	}
	text_data& Get_Child(const wchar_t* index){
		return *m_child[index];
	}
	text_data& Get_Child(int index){
		map<wstring,text_data*>::iterator itr;
		for(itr=m_child.begin();itr!=m_child.end();itr++){
			if(index==0){
				return *itr->second;
			}
			index--;
		}
	}
	*/
};

class td_datas{
	struct td_data{
		int begin,end;
		HFONT hFont;
		COLORREF back_color,text_color;
	};
	vector<td_data> data;
};

class text_draw:public text_data{
private:
	struct fc_data{
		HFONT hFont;
		COLORREF tx,bk;
	};
	wstring m_text;
public:
	text_data m_data;
public:
	void SetText(const wchar_t* text){
		m_text.assign(text);
	}
	void Draw(HDC hdc){
		SIZE tmp;
		int width=0;
		wstring draw_text;
		vector<draw_font_data>dfd;
		HFONT oldfont=(HFONT)GetCurrentObject(hdc,OBJ_FONT);
		m_data.Get_Area_Datas(&dfd);

		int i;
		for(i=0;i<dfd.size();i++){
			draw_text.assign(m_text,dfd[i].begin,dfd[i].end-dfd[i].begin);
			SelectObject(hdc,dfd[i].hFont);
			TextOut(hdc,width,0,draw_text.c_str(),draw_text.size());
			GetTextExtentPoint32(hdc,draw_text.c_str(),draw_text.size(),&tmp);
			width+=tmp.cx;
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