#include "MAIN_VIEWER.h"


DrawTextData::DrawTextData(){
	m_hFont=NULL;
	m_text.clear();
}

DrawTextData::~DrawTextData(){
	Reset();
}

void DrawTextData::Reset(){
	if(m_hFont!=NULL){
		::DeleteObject(m_hFont);
		m_hFont=NULL;
	}
	return;
}

void DrawTextData::SetFontDirect(LOGFONT*lf){
	Reset();
	m_hFont=CreateFontIndirect(lf);
	return;
}

void DrawTextData::SetFont(int n_size,const wchar_t* face,bool b_italic,bool b_underline,bool b_strike){
	LOGFONT lf;
	memset(&lf,0,sizeof(lf));
	lf.lfHeight=n_size;
	lf.lfItalic=b_italic;
	lf.lfUnderline=b_underline;
	lf.lfStrikeOut=b_strike;
	lstrcpyn(lf.lfFaceName,face,sizeof(lf.lfFaceName)/sizeof(wchar_t));

	SetFontDirect(&lf);
	return;
}

void DrawTextData::SetArea(int left,int top,int right,int bottom){
	m_rc.left=left;
	m_rc.top=top;
	m_rc.right=right;
	m_rc.bottom=bottom;
	return;
}

void DrawTextData::SetText(const wchar_t* text){
	m_text.assign(text);
	return;
}

void DrawTextData::Draw(HDC hdc){
	SetBkMode(hdc,TRANSPARENT);
	TextOut(hdc,m_rc.left,m_rc.top,m_text.c_str(),m_text.size());
	return;
}

void DrawTextData::SetNewArea(int begin,int end,const wchar_t* id_str){
	vector<pair<int,int>> areas=m_areas[id_str].areas;
	areas.clear();
	areas.push_back(pair<int,int>(begin,end));
	m_areas[id_str].areas=areas;
}

void DrawTextData::Area_Not(int begin,int end,const wchar_t* id_str){
	//•Ï‚ÈID‚Ì‚Íˆ—‚µ‚È‚¢
	if(m_areas.count(id_str)==0){
		return;
	}
	//‚¨‚©‚µ‚È’l‚Ì‚Íˆ—‚µ‚È‚¢
	if(end<=begin){
		return;
	}
	vector<pair<int,int>> *areas=&m_areas[id_str].areas;
	vector<pair<int,int>>::iterator itr;
	int i;
	pair<int,int> tmp;
	
	for(itr=areas->begin();itr!=areas->end();){
		if(begin<(*itr).second&&(*itr).first<end){
			tmp=*itr;
			itr=areas->erase(itr);
			if(tmp.first<begin){
				itr=areas->insert(itr,pair<int,int>(tmp.first,begin));
				itr++;
			}
			if(end<tmp.second){
				itr=areas->insert(itr,pair<int,int>(end,tmp.second));
				itr++;
			}
		}else{
			itr++;
		}
	}
	/*
	vector<pair<int,int>> areas=m_areas[id_str].areas;
	int i;
	pair<int,int> tmp;
	
	for(i=0;i<areas.size();){
		if(begin<areas[i].second&&areas[i].first<end){
			tmp=areas[i];
			areas.erase(areas.begin()+i);
			if(tmp.first<begin){
				areas.insert(areas.begin()+i,pair<int,int>(tmp.first,begin));
				i++;
			}
			if(end<tmp.second){
				areas.insert(areas.begin()+i,pair<int,int>(end,tmp.second));
				i++;
			}
		}else{
			i++;
		}
	}
	*/
	return;
}

void DrawTextData::Area_Or(int begin,int end,const wchar_t* id_str){
	if(m_areas.count(id_str)==0){
		return;
	}
	if(end<=begin){
		return;
	}
	vector<pair<int,int>> *areas=&m_areas[id_str].areas;
	vector<pair<int,int>>::iterator itr;
	pair<int,int> tmp,result;
	result.first=begin;
	result.second=end;
	int i;
	for(itr=areas->begin();itr!=areas->end();){
		if(result.first<itr->second&&itr->first<result.second){
			tmp=*itr;
			itr=areas->erase(itr);
			result.first=min(result.first,tmp.first);
			result.second=max(result.second,tmp.second);
		}else{
			itr++;
		}
	}
	areas->push_back(result);
	/*
	vector<pair<int,int>> areas=m_areas[id_str].areas;
	pair<int,int> tmp,result;
	result.first=begin;
	result.second=end;
	int i;
	for(i=0;i<areas.size();){
		if(result.first<areas[i].second&&areas[i].first<result.second){
			tmp=areas[i];
			areas.erase(areas.begin()+i);
			result.first=min(result.first,tmp.first);
			result.second=max(result.second,tmp.second);
		}else{
			i++;
		}
	}
	areas.push_back(result);
	*/
	return;
}


void DrawTemplate::Set(const wchar_t* text){
	m_textdata.SetArea(0,0,100,100);
	m_textdata.SetFont(20,TEXT("ƒƒCƒŠƒI"),false,false,false);
	m_textdata.SetText(text);
}

void DrawTemplate::Set(int left,int top,int right,int bottom){
	m_rc.left=left;
	m_rc.right=right;
	m_rc.top=top;
	m_rc.bottom=bottom;
}

void DrawTemplate::Draw(HWND hWnd){
	HDC hdc;
	PAINTSTRUCT ps;
	hdc=BeginPaint(hWnd,&ps);
	SelectObject(hdc,hFont);

	FillRect(hdc,&m_rc,(HBRUSH)GetStockObject(LTGRAY_BRUSH));
	SetBkMode(hdc,TRANSPARENT);
	TextOut(hdc,m_rc.left,m_rc.top,m_text.c_str(),m_text.size());
	EndPaint(hWnd,&ps);
}

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
	static DrawTemplate dt;
	switch(msg){
	case WM_CREATE:
		{
			DrawTextData dtd;
			dtd.SetNewArea(0,3,TEXT("a"));
			dtd.Area_Or(2,9,TEXT("a"));
			dtd.Area_Not(4,6,TEXT("a"));
		}
		return 0;
	case WM_MOUSEMOVE:
		return 0;
	case WM_PAINT:
		dt.Draw(hWnd);
		return 0;
	case WM_LBUTTONDOWN:
		dt.Set(TEXT("TEXTETXETEHRH"));
		dt.Set(10,40,190,130);
		return 0;
	case WM_LBUTTONUP:
		return 0;
	case WM_SIZE:
		MoveWindow(hWnd,0,0,400,400,TRUE);
		return 0;
	}
	return DefWindowProc(hWnd,msg,wp,lp);
}

