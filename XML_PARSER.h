#ifndef XML_PARSER
#define XML_PARSER
#include "WinMain.h"
#include <string.h>
#include <stack>
#include <string>
#include <list>
#include <vector>
#include <map>
using namespace std;

int find_multi(wchar_t*,wchar_t*,int,wchar_t*);
int find_front(wchar_t*,wchar_t*,int);
int find_back(wchar_t*,wchar_t*,int);

class Xml_DataPair{
private:
public:
	wstring key;
	wstring data;
public:
	Xml_DataPair();
	Xml_DataPair(const wchar_t*,const wchar_t*);
	void Get(wstring*,wstring*);
	Xml_DataPair Set(const wchar_t*,const wchar_t*);
};
class Tree{
public:
	wstring elem;
	wstring str;
	Tree* parent;
	list<Tree*> node;
	vector<Xml_DataPair> Attrib;
public:
	Tree();
	Tree(const wchar_t*,const wchar_t*);
	//Tree(Tree& src){
	//	*this=src;
	//}
	~Tree();
	Tree* AddNode(Tree*);
	Tree* GetNode_n(int);
	Tree* GetNode_str(const wchar_t*);
	Tree* GetParent();
};

//blank=区切り。記号に連続で続くなら無視する
//word=普通の記号
//str=文字列。終わりの記号が見つかるまで記号を無視する
class opera{
public:
	wstring m_blank,m_word;
	wstring m_str[2];
public:
	opera Set(wchar_t*blank,wchar_t*word,wchar_t*pbeg,wchar_t*pend){
		m_blank=blank;
		m_word=word;
		m_str[0]=pbeg;
		m_str[1]=pend;
		return *this;
	}
};

class opera_am{
public:
	map<wstring,opera_am*> next;
public:
	opera_am(){
		
	}
	opera_am* Add(const wchar_t* key){
		opera_am* res;
		//secondに成功判定(bool)がはいってる（すでに同名のキーが存在するなら、失敗）
		if(next.insert(pair<wstring,opera_am*>(key,new opera_am)).second){
			res=next.find(key)->second;
		}else{
			return NULL;
		}
		return res;
	}
	opera_am* GetNext(const wchar_t* key){
		map<wstring,opera_am*>::iterator itr;
		for(itr=next.begin();itr!=next.end();itr++){
			if(wcsncmp(itr->first.c_str(),key,itr->first.size())==0){
				return itr->second;
			}
		}
		return NULL;
	}
	~opera_am(){
		map<wstring,opera_am*>::iterator itr;
		for(itr=next.begin();itr!=next.end();itr++){
			delete itr->second;
		}
		next.clear();
	}
};

struct current{
	wchar_t* str;
	unsigned long long n_curnt;
	int n_line;
	int n_ch;
	Tree tree;
	Tree *c_tree;
	
	
	current(){};

	void Init(wchar_t* src){
		str=src;
		n_line=0;
		n_ch=0;
		n_curnt=0;
		tree.parent=NULL;
		c_tree=&tree;
	}

	wchar_t* getcurrentstr(){
		return str+n_curnt;
	}

	void inclement(int size=1){
		int len=n_curnt+size;
		for(;n_curnt<len;n_curnt++){
			if(str[n_curnt]=='\n'){
				n_line++;
				n_ch=0;
			}else{
				n_ch++;
			}
		}
		return;
	}
};

class Xml_Parser{
	Tree root;
	stack<wchar_t*>parant;
	current m_current;
	bool comment;
	vector<Xml_DataPair>entity;

	void Init();

	int reference(current*,int,wstring*);
	int char_str(current*,int,wchar_t,wstring*);
	int attribute(current*,int,Xml_DataPair*);
	int element(current*);

	bool bifurc(current*);

	bool get_symbol(current*,int,wchar_t*,wstring*);
	bool get_opera(current*,int,wchar_t*,wchar_t*,wchar_t*);

	bool split(current*,opera_am*,int,vector<wstring>*);

	void spcskip(current*);
	bool scanning(current*,wchar_t*,int,wchar_t*);
	bool namerule(const wchar_t*,int,int*);
public:
	void Parse(wchar_t*);
};

#endif