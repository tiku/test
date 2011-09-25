#ifndef JSON_PARSER
#define JSON_PARSER


#include <Windows.h>
#include<iostream>
#include <list>
#include <vector>
#include <string>
#include <stack>

using namespace std;

/*éQçl
http://coderepos.org/share/browser/lang/cplusplus/picojson/trunk/picojson.h#L232
*/

#define NUM_HEX 16
#define SURROGATE_MIN		0x10000
#define SURROGATE_HIGH_MIN	0xD800
#define SURROGATE_LOW_MIN	0xDC00
#define SURROGATE_HIGH_MAX	0xDBFF
#define SURROGATE_LOW_MAX	0xDFFF

#define SYNTAX_NULL		TEXT("null")
#define SYNTAX_TRUE		TEXT("true")
#define SYNTAX_FALSE	TEXT("false")


class Json_Obj;
class Json_Arr;

class Value{
	enum{NULL_TYPE,NUMBER_TYPE,BOOL_TYPE,STRING_TYPE,OBJECT_TYPE,ARRAY_TYPE};
	union data_union{
		double val_num;
		bool val_bool;
		wstring* val_str;
		Json_Obj* val_obj;
		Json_Arr* val_arr;
		//vector<pair<wstring,Value>>* val_obj;
		//vector<Value>* val_arr;
	};
	data_union data;
	int CType;
public:
	Value();
	~Value();
	Value(const Value& src);
	Value& operator=(const Value& src){
		this->CType=src.CType;
		this->data=src.data;
		return *this;
	}
	void Init();
	void Reset();
	void Set(double num);
	void Set(bool boolian);
	void Set();
	void Set(const wchar_t*str);
	void Set(Json_Obj obj);
	void Set(Json_Arr arr);
	template<class T>bool Get(T*); 
	bool Is(int,void*);
	bool Get(double*);
	bool Get(bool*);
	bool Get();
	bool Get(wstring*);
	bool Get(Json_Obj*);
	bool Get(Json_Arr*);
	void copy(Value* dst,Value* src);
};

class Json_Obj{
public:
	vector<pair<wstring,Value>> elements;
};

class Json_Arr{
public:
	vector<Value> elements;
};

typedef pair<wstring,wstring> JSON_PairData;
class Json_Tree{
public:
	vector<wstring> arr;
	vector<JSON_PairData> obj;
	Json_Tree*Parent;
	list<Json_Tree*> Node;
};

class Json_Parser{
	class json_struct{
	private:
		wchar_t* src;
		unsigned long long index;
	public:
		Value* current;
		Value root;
	public:
		json_struct(){
			Init(TEXT(""));
		}
		void Init(wchar_t* str){
			src=str;
			index=0;
		}
		wchar_t inclement(int inc=1){
			index+=inc;
			return src[index];
		}
		wchar_t* getcurrentstr(){
			return src+index;
		}
		wchar_t get(){
			return *getcurrentstr();
		}
		wchar_t getc(){
			inclement();
			return *getcurrentstr();
		}
		
		bool ignore(wchar_t dl,wstring* res){
			wchar_t c;
			while(inclement()!=NULL){
				res->push_back(get());
				if(get()==dl){
					inclement();
					return true;
				}
			}
			return false;
		}
	};

	json_struct json_str;
	Value root;
	Value*current;

	bool Parse_CodePoint(json_struct*,wchar_t*);
	bool Parse_text(json_struct*,wstring*);
	bool Parse_num(json_struct*,double*);
	bool Parse_Obj(json_struct*,Json_Obj*);
	bool Parse_Arr(json_struct*,Json_Arr*);

	bool Parse_Value(json_struct*,Value*);

	wchar_t spcskip(json_struct*);
	void Error(const wchar_t* msg,json_struct* js){
		wstring err;
		err.resize(lstrlen(js->getcurrentstr())+lstrlen(msg)+100);
		wsprintf((wchar_t*)err.c_str(),TEXT("msg:%s\nline:%d,column:%d,Str:...%s"),msg,0,0,js->getcurrentstr());
		MessageBox(NULL,err.c_str(),NULL,MB_OK);
	}
public:
	//~Json_Parser();
	bool Parse(wchar_t*);
};

#endif