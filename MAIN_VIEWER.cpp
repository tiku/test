#include "MAIN_VIEWER.h"


void area_data::SetFont(int n_size,const wchar_t* face,bool b_italic,bool b_underline,bool b_strike){
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

void area_data::SetFontDirect(LOGFONT *lf){
	if(m_hFont){
		DeleteObject(m_hFont);
		m_hFont=NULL;
	}
	if(lf){
		m_hFont=CreateFontIndirect(lf);
	}
	return;
}

void area_data::SetColor(COLORREF tx,COLORREF bk){
	text_color=tx;
	bk_color=bk;
	return;
}

void area_data::Area_Set(int begin,int end){
	m_area.assign(1,pair<int,int>(begin,end));
}

void area_data::Area_And(int begin,int end){
	if(end<=begin){
		return;
	}
	vector<pair<int,int>>::iterator itr;
	pair<int,int>tmp;

	for(itr=m_area.begin();itr!=m_area.end();){
		if(begin<itr->second&&itr->first<end){
			itr->first=max(itr->first,begin);
			itr->second=min(itr->second,end);
			itr++;
		}else{
			itr=m_area.erase(itr);
		}
	}
}

void area_data::Area_Not(int begin,int end){
	if(end<=begin){
		return;
	}
	vector<pair<int,int>>::iterator itr;
	pair<int,int> tmp;
	
	for(itr=m_area.begin();itr!=m_area.end();){
		if(begin<(*itr).second&&(*itr).first<end){
			tmp=*itr;
			itr=m_area.erase(itr);
			if(tmp.first<begin){
				itr=m_area.insert(itr,pair<int,int>(tmp.first,begin));
				itr++;
			}
			if(end<tmp.second){
				itr=m_area.insert(itr,pair<int,int>(end,tmp.second));
				itr++;
			}
		}else{
			itr++;
		}
	}
}

void area_data::Area_Or(int begin,int end){
	if(end<=begin){
		return;
	}
	vector<pair<int,int>>::iterator itr;
	pair<int,int> tmp,result;

	result.first=begin;
	result.second=end;

	for(itr=m_area.begin();itr!=m_area.end();){
		if(result.first<itr->second&&itr->first<result.second){
			tmp=*itr;
			itr=m_area.erase(itr);
			result.first=min(result.first,tmp.first);
			result.second=max(result.second,tmp.second);
		}else{
			itr++;
		}
	}
	m_area.push_back(result);
}

void area_data::Draw(HDC hdc){
	HFONT oldfont;
	oldfont=(HFONT)GetObject(hdc,sizeof(HFONT),&oldfont);

	SelectObject(hdc,m_hFont);
	SetBkColor(hdc,bk_color);
	SetTextColor(hdc,text_color);

}


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

	return;
}

void DrawTextData::SetNewArea(int begin,int end,const wchar_t* id_str){
	vector<pair<int,int>> areas=m_areas[id_str].m_area;
	areas.clear();
	areas.push_back(pair<int,int>(begin,end));
	m_areas[id_str].m_area=areas;
}

void DrawTextData::Area_Not(int begin,int end,const wchar_t* id_str){
	//変なIDの時は処理しない
	if(m_areas.count(id_str)==0){
		return;
	}
	//おかしな値の時は処理しない
	if(end<=begin){
		return;
	}
	vector<pair<int,int>> *areas=&m_areas[id_str].m_area;
	vector<pair<int,int>>::iterator itr;
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
	return;
}

void DrawTextData::Area_Or(int begin,int end,const wchar_t* id_str){
	if(m_areas.count(id_str)==0){
		return;
	}
	if(end<=begin){
		return;
	}
	vector<pair<int,int>> *areas=&m_areas[id_str].m_area;
	vector<pair<int,int>>::iterator itr;
	pair<int,int> tmp,result;
	result.first=begin;
	result.second=end;

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
	return;
}

void DrawTextData::Area_And(int begin,int end,const wchar_t* id_str){
	if(m_areas.count(id_str)==0){
		return;
	}
	if(end<=begin){
		return;
	}
	vector<pair<int,int>> *areas=&m_areas[id_str].m_area;
	vector<pair<int,int>>::iterator itr;
	pair<int,int>tmp;

	for(itr=areas->begin();itr!=areas->end();){
		if(begin<itr->second&&itr->first<end){
			itr->first=max(itr->first,begin);
			itr->second=min(itr->second,end);
			itr++;
		}else{
			itr=areas->erase(itr);
		}
	}
	return;
}


void DrawTemplate::Set(const wchar_t* text){
	m_textdata.SetArea(0,0,100,100);
	m_textdata.SetFont(20,TEXT("メイリオ"),false,false,false);
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
			text_data td[10000];
			int i;
			for(i=0;i<10000;i++){
			//	td[i].AddChild(TEXT("a"));
				td[i].SetFont(30,TEXT("メイリオ"),true,true,true);
				td[i].SetColor(RGB(0XFF,0XFF,0XFF),RGB(0XAA,0XAA,0XAA));
			//	td[i][TEXT("a")].AddChild(TEXT("b"));
			//	td[i][TEXT("a")].AddChild(TEXT("c"));
			//	td[i][TEXT("a")].SetFont(29,TEXT("メイリオ"),false,false,false);
			//	td[i][TEXT("a")][TEXT("b")].SetFont(30,TEXT("ＭＳゴシック"),true,false,false);
			}
			MessageBox(NULL,NULL,NULL,NULL);
		}
		{
			area_data ad[10000];
			int i=0;
			for(i=0;i<10000;i++){
				ad[i].SetFont(30,TEXT("メイリオ"),true,true,true);
			}
			MessageBox(NULL,NULL,NULL,NULL);
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

