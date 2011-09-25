#include "JSON_PARSER.h"


Value::Value(){
	Init();
}
Value::~Value(){
	Reset();
}
Value::Value(const Value& src){
	Init();
	CType=src.CType;
	switch(CType){
	case NULL_TYPE:
		Set();
		break;
	case NUMBER_TYPE:
		Set(src.data.val_num);
		break;
	case BOOL_TYPE:
		Set(src.data.val_bool);
		break;
	case STRING_TYPE:
		Set(src.data.val_str->c_str());
		break;
	case OBJECT_TYPE:
		Set(*src.data.val_obj);
		break;
	case ARRAY_TYPE:
		Set(*src.data.val_arr);		
		break;
	default:
		;
	}
}


void Value::Init(){
	CType=NULL_TYPE;
	memset(&data,0,sizeof(data));
}

void Value::Reset(){
	void *ptr=NULL;

	switch(CType){
	case STRING_TYPE:
		ptr=data.val_str;
		break;
	case OBJECT_TYPE:
		ptr=data.val_obj;
		break;
	case ARRAY_TYPE:
		ptr=data.val_arr;
		break;
	}
	if(ptr){
		delete ptr;
		ptr=NULL;
	}
}

void Value::Set(double num){
	Reset();
	data.val_num=num;
	CType=NUMBER_TYPE;
}
void Value::Set(bool boolian){
	Reset();
	data.val_bool=boolian;
	CType=BOOL_TYPE;
}
void Value::Set(const wchar_t* str){
	Reset();
	data.val_str=new wstring(str);
	CType=STRING_TYPE;
}
void Value::Set(Json_Obj obj){
	Reset();
	data.val_obj=new Json_Obj(obj);
	CType=OBJECT_TYPE;
}
void Value::Set(Json_Arr arr){
	Reset();
	data.val_arr=new Json_Arr(arr);
	CType=ARRAY_TYPE;
}
void Value::Set(){
	Reset();
	CType=NULL_TYPE;
}
/*
void Value::copy(Value* dst,Value* src){
	switch(src->CType){
	case STRING_TYPE:
		{
			wstring tmp;
			src->Get(&tmp);
			dst->Set(tmp.c_str());
		}
		break;
	case OBJECT_TYPE:
		{
			//Json_Obj* obj;
			//src->Get(obj);
			//dst->Set(*obj);
		}
		break;
	case ARRAY_TYPE:
		{
			//Json_Arr* arr;
			//src->Get(arr);
			//dst->Set(*arr);
		}
		break;
	default:
		*dst=*src;
	}
}
*/
bool Json_Parser::Parse_CodePoint(json_struct *json_str,wchar_t* res){
	wstring cp_str;
	//wstring res;
	unsigned long cp_num;
	bool surrogate=false;
	wchar_t high,low;
	
	if(wcsncmp(json_str->getcurrentstr(),TEXT("\\u"),lstrlen(TEXT("\\u")))!=0){
		//return false;
	}
	json_str->inclement(lstrlen(TEXT("\\u")));

	
	while(isxdigit(json_str->get())&&cp_str.size()<4){
		cp_str.push_back(json_str->get());
		json_str->inclement();
	}
	cp_num=wcstol(cp_str.c_str(),NULL,NUM_HEX);
	if(0xffff<cp_num){
		Error(TEXT("コードポイントの数値がおかしいです"),json_str);
		return false;
	}
	*res=(wchar_t)cp_num;
	return true;
	/*
	if(SURROGATE_MIN<=cp_num){
		//surrogate_pair
		cp_num-=SURROGATE_MIN;
		high=cp_num>>10;
		low=cp_num&0x3FF;//0x3ff=0x11 1111 1111

		high+=SURROGATE_HIGH_MIN;
		low+=SURROGATE_LOW_MIN;
		if((high<0||SURROGATE_HIGH_MAX<high)||(low<0||SURROGATE_LOW_MAX<low)){
			//ERRROR
		}
		res.push_back(high);
		res.push_back(low);
	}else{
		res.push_back(cp_num);
	}
	*/
}

wchar_t Json_Parser::spcskip(json_struct* json_str){
	wchar_t* spc=TEXT("\r\t\n ");
	while(wcschr(spc,json_str->get())){
		json_str->inclement();
	}
	return json_str->get();
}

bool Json_Parser::Parse_text(json_struct* json_str,wstring *res){
	wchar_t*text;
	int i;
	wchar_t *pch;
	wchar_t cp_tmp;
	wstring res_str;

	if(json_str->get()!='"'){
		//ERROR;
	}
	json_str->inclement();
	while(1){
		pch=json_str->getcurrentstr();
		if(*pch=='"'){
			json_str->inclement();
			break;
		}else if(*pch=='\\'){
			if(wcsncmp(pch,TEXT("\\u"),lstrlen(TEXT("\\u")))==0){
				Parse_CodePoint(json_str,&cp_tmp);
				res_str.push_back(cp_tmp);
			}else{
				switch(json_str->getc()){
				case 'b':
				case 'f':
				case 'r':
				case 'n':
				case 't':
				case '\\':
				case '"':
					res_str.push_back('\\');
					res_str.push_back(json_str->get());
					json_str->inclement();
					break;
				case '/':
					res_str.push_back(json_str->get());
					json_str->inclement();
					break;
				default:
					Error(TEXT("予期しないエスケープシーケンスです"),json_str);
					return false;
				}
			}
		}else{
			res_str.push_back(*pch);
			json_str->inclement();
		}
	}
	*res=res_str;
	return true;
}

bool Json_Parser::Parse_num(json_struct* json_str,double* res){
	wstring tmp;
	wchar_t *ds=TEXT("0123456789-+.eE");
	wchar_t *err=NULL;
	wchar_t ch;
	double n_res;
	while(1){
		ch=json_str->get();
		if(wcschr(ds,ch)==NULL){
			break;
		}
		tmp.push_back(ch);
		json_str->inclement();
	}

	n_res=wcstod(tmp.c_str(),&err);
	if(lstrlen(err)!=0){
		Error(TEXT("数値がおかしいです"),json_str);
		return false;
	}
	*res=n_res;
	return true;
}

bool Json_Parser::Parse_Value(json_struct*json_str,Value*res){
	//bool ret=false;
	bool ret;
	wchar_t *ch=json_str->getcurrentstr();
	//無理矢理
	if(ch==NULL){
		return true;
	}

	if(*ch=='"'){//double quotation
		wstring tmp;
		ret=Parse_text(json_str,&tmp);
		res->Set(tmp.c_str());
	}else if(isdigit(*ch)||*ch=='-'){//minus and number
		double tmp;
		ret=Parse_num(json_str,&tmp);
		res->Set(tmp);
	}else if(wcsncmp(ch,SYNTAX_NULL,lstrlen(SYNTAX_NULL))==0){
		res->Set();
		json_str->inclement(lstrlen(SYNTAX_NULL));
		ret=true;
	}else if(wcsncmp(ch,SYNTAX_TRUE,lstrlen(SYNTAX_TRUE))==0){
		json_str->inclement(lstrlen(SYNTAX_TRUE));
		res->Set(true);
		ret=true;
	}else if(wcsncmp(ch,SYNTAX_FALSE,lstrlen(SYNTAX_FALSE))==0){
		json_str->inclement(lstrlen(SYNTAX_FALSE));
		res->Set(false);
		ret=true;
	}else if(*ch=='['){
		Json_Arr arr;
		ret=Parse_Arr(json_str,&arr);
		res->Set(arr);
	}else if(*ch=='{'){
		Json_Obj obj;
		ret=Parse_Obj(json_str,&obj);
		res->Set(obj);
	}else if(isspace(*ch)){
		json_str->inclement();
		ret=Parse_Value(json_str,res);
	}else if(*ch=='\0'){
		ret=true;
	}else{
		ret=false;//ERROR
		Error(TEXT("予期しない構文です(Value)"),json_str);
	}
	return ret;
}

bool Json_Parser::Parse_Arr(json_struct*json_str,Json_Arr*res){
	Json_Arr arr;
	Value tmp;

	if(json_str->get()!='['){
		//ERROR
	}
	
	json_str->inclement();
	if(spcskip(json_str)==']'){
		json_str->inclement();
		*res=arr;
		return true;
	}
	while(1){
		if(!Parse_Value(json_str,&tmp)){
			return false;
		}
		arr.elements.push_back(tmp);
		if(spcskip(json_str)==','){
			json_str->inclement();
			spcskip(json_str);
		}else{
			break;
		}
	}

	if(json_str->get()!=']'){
		Error(TEXT("'['が閉じられてません"),json_str);
		return false;
	}
	json_str->inclement();
	*res=arr;
	return true;

	/*wchar_t ch;
	wchar_t prev;
	if(json_str->get()!=OPERA_ARRAY_START){
		//ERROR;
	}
	json_str->inclement();
	Json_Arr arr;
	Value tmp;
	vector<Value> a;

	//opera.push('[');
	prev=OPERA_ARRAY_START;
	while(1){
		ch=json_str->get();
		switch(ch){
		case ']':
			if(prev!=OPERA_ARRAY_START&&prev==OPERA_VALUE){
				//error
			}
			json_str->inclement();
			*res=arr;
			return;
		case ',':
			if(prev!=OPERA_VALUE){
				//ERROR
			}
			prev=OPERA_DELIMITA;
			json_str->inclement();	
			break;
		default:
			if(isspace(ch)){
				json_str->inclement();
				break;
			}
			if(prev==OPERA_VALUE){
				//ERROR
			}
			Parse_Value(json_str,&tmp);
			arr.elements.push_back(tmp);
			prev=OPERA_VALUE;
		}//siwtch
	}//while
	return;
	*/
}//function

bool Json_Parser::Parse_Obj(json_struct*json_str,Json_Obj *res){
/*
	wchar_t *pch=json_str->getcurrentstr();
	wchar_t prev;
	stack<wchar_t>opera;
	if(json_str->get()!=OPERA_OBJECT_START){
		//ERROR
	}
	json_str->inclement();
	Json_Obj obj;
	pair<wstring,Value> pd;

	prev=OPERA_OBJECT_START;
	Value tmp;
	while(1){
		pch=json_str->getcurrentstr();
		switch(*pch){
		case '}':
			if(opera.top()!='{'){
				//ERROR
			}
			json_str->inclement();
			*res=obj;
			return;
		case ',':
			if(opera.top()!='{'){
				//ERROR
			}
			//opera.pop();
			json_str->inclement();
			break;
		case ':':
			if(opera.top()!='"'){
				//ERROR
			}
			opera.pop();
			opera.push(':');
			json_str->inclement();
			break;
		default:
			if(isspace(*pch)){
				json_str->inclement();
				break;
			}
			if((*pch)=='"'&&opera.top()=='{'){
				Parse_text(json_str,&pd.first);
				opera.push('"');
			}else{
				if(opera.top()!=':'){
						//ERROR
				}
				Parse_Value(json_str,&pd.second);
				obj.elements.push_back(pd);
				opera.pop();
			}
			break;
		}
	}
	return;
	*/
	Json_Obj obj;
	pair<wstring,Value> pd;

	if(json_str->get()!='{'){
		//ERROR
	}

	json_str->inclement();
	if(spcskip(json_str)=='}'){
		json_str->inclement();
		*res=obj;
		return true;
	}
	
	while(1){
		if(spcskip(json_str)!='"'){
			Error(TEXT("予期しない構文です(Object)"),json_str);
			return false;
		}
		Parse_text(json_str,&pd.first);
		if(spcskip(json_str)!=':'){
			Error(TEXT("予期しない構文です(Object)"),json_str);
			return false;
		}
		json_str->inclement();
		spcskip(json_str);

		Parse_Value(json_str,&pd.second);
		obj.elements.push_back(pd);

		if(spcskip(json_str)==','){
			json_str->inclement();
			spcskip(json_str);
		}else{
			break;
		}
	}


	/*do{
		json_str->inclement();
		if(spcskip(json_str)!='"'){
			Error(TEXT("予期しないsyntaxです"),json_str);
			return false;
		}
		Parse_text(json_str,&pd.first);
		if(spcskip(json_str)!=':'){
			Error(TEXT("予期しないsyntaxです"),json_str);
			return false;
		}
		json_str->inclement();
		spcskip(json_str);

		Parse_Value(json_str,&pd.second);
		obj.elements.push_back(pd);
	}while(spcskip(json_str)==',');*/

	if(json_str->get()!='}'){
		Error(TEXT("括弧'{'が閉じられてません"),json_str);
		return false;
	}
	json_str->inclement();
	*res=obj;
	return true;
}


bool Json_Parser::Parse(wchar_t* src){
	json_str.Init(src);
	//*current=root;*/
	return Parse_Value(&json_str,&root);
	
}

