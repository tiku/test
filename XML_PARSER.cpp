#include "XML_PARSER.h"


Xml_DataPair::Xml_DataPair(){
	key=TEXT("");
	data=TEXT("");
}

Xml_DataPair Xml_DataPair::Set(const wchar_t* k,const wchar_t*d){
	key=k;
	data=d;
	return *this;
}

void Xml_DataPair::Get(wstring* resk,wstring *resd){
	*resk=key;
	*resd=data;
	return;
}

Tree::Tree(){
	
}
Tree::Tree(const wchar_t* k,const wchar_t* d){
	
}
Tree::~Tree(){
	list<Tree*>::iterator itr;
	for(itr=node.begin();itr!=node.end();itr++){
		delete *itr;
	}
	node.clear();
}

Tree* Tree::GetNode_n(int index){
	list<Tree*>::iterator itr;
	if(index<0&&node.size()<=index){
		return NULL;
	}
	for(itr=node.begin();itr!=node.end();itr++,index--){
		if(index==0){
			return *itr;
		}
	}
	return NULL;
}

//mapは重複は許されないので次を調べる必要はない
Tree* Tree::GetNode_str(const wchar_t* str){
	list<Tree*>::iterator itr;
	for(itr=node.begin();itr!=node.end();itr++){
		if((*itr)->elem==str){
			return *itr;
		}
	}
	return NULL;
}

Tree* Tree::AddNode(Tree* nl){
	node.insert(node.end(),nl);
	nl->parent=this;
	return nl;
}

Tree* Tree::GetParent(){
	return parent;
}

void Xml_Parser::Init(){
	Xml_DataPair null;
	wchar_t*entitys[][3]={
		{TEXT("amp"),TEXT("&")},
		{TEXT("lt"),TEXT("<")},
		{TEXT("gt"),TEXT(">")},
		{TEXT("apos"),TEXT("'")},
		{TEXT("quot"),TEXT("\"")}
	};
	int i;
	for(i=0;i<sizeof(entitys)/sizeof(entitys[0]);i++){
		null.Set(entitys[i][0],entitys[i][1]);
		entity.push_back(null);
	}
}

bool Xml_Parser::namerule(const wchar_t* src,int alen,int* erindex){
	int i,len,er;
	len=lstrlen(src);
	if(isdigit(src[0])||src[0]=='.'||src[0]=='-'){
		if(erindex!=NULL){
			*erindex=0;
		}
		return false;
	}
	for(i=0;i<len;i++){
		if(
			((0x0020<=src[i]&&src[i]<=0x00BF)&&!isalnum(src[i])&&src[i]!='.'&&src[i]!='_'&&src[i]!='-')||
			(0x2000<=src[i]&&src[i]<=0x3040)||
			(0x3097<=src[i]&&src[i]<=0x30A0)||
			(0x30FB<=src[i]&&src[i]<=0x33E0)||
			(0xFE00<=src[i]&&src[i]<=0xFFE0)
			)
		{
			if(erindex!=NULL){
				*erindex=i;
			}
			return false;
		}

	}
	return true;
}
/*
int Xml_Parser::find_front(wchar_t* src,wchar_t *pat,int len){
	int i,j,plen;
	plen=lstrlen(pat);
	i=0;
	while(i<len){
		if(src[i]=='\0'){
			break;
		}
		j=0;
		while(1){
			if(src[i+j]!=pat[j]){
				break;
			}
			j++;
			if(j==plen){
				return i;
			}
		}
		i++;
	}
	return string::npos;
}

int Xml_Parser::find_back(wchar_t* src,wchar_t *pat,int len){
	int res=find_front(src,pat,len);
	return (res==string::npos)?(string::npos):(res+lstrlen(pat));
}

int Xml_Parser::find_multi(wchar_t* src,wchar_t* pats,int len,wchar_t* res){
	int i,j,plen;
	plen=lstrlen(pats);
	
	for(i=0;i<len;i++){
		for(j=0;j<plen;j++){
			if(src[i]==pats[j]){
				*res=pats[j];
				return i;
			}
		}
	}
	*res=string::npos;
	return string::npos;
}
*/
int Xml_Parser::reference(current* current,int len,wstring* res){
	wstring ref;
	wchar_t* er,*src;
	int i,num;
	bool hex=false;

	if(current->str[current->n_curnt]!='&'){
		return false;
	}
	current->inclement();
	get_symbol(current,len,TEXT(";"),&ref);
	if(ref[0]=='#'){
		i=1;
		if(ref[1]=='x'){
			i++;
			hex=true;
		}
		num=wcstol(ref.c_str()+i,&er,hex?16:10);
		res->append(1,(wchar_t)num);
		return true;
	}else{
		for(i=0;i<entity.size();i++){
			if(entity[i].key==(current->str+current->n_curnt)){
				res->append(entity[i].data);
				return true;
			}
		}
	}
	//ERROR!
	return false;
}

//区切り文字を読み飛ばす
void Xml_Parser::spcskip(current* current){
	wchar_t cur;
	while(1){
		cur=*current->getcurrentstr();
		if(isspace(cur)){
			current->inclement();
		}else{
			break;
		}
	}

}

//指定文字が現れるまで走査する
bool Xml_Parser::scanning(current* current,wchar_t* pats,int len,wchar_t* res){
	int j,clen;
	wchar_t cur;

	clen=lstrlen(pats);
	while(current->n_curnt<len){
		cur=*current->getcurrentstr();

		for(j=0;j<clen;j++){
			if(cur==pats[j]){
				*res=pats[j];
				return true;
			}
		}
		current->inclement();
	}
	*res=string::npos;
	return false;
}


int Xml_Parser::char_str(current* current,int len,wchar_t end,wstring* res){
	wchar_t ch;

	while(current->n_curnt<len){
		ch=*current->getcurrentstr();
		if(ch==end){
			return true;
		}else if(ch=='&'){
			reference(current,len,res);
		}else{
			res->append(1,ch);
		}
		current->inclement();
	}
	return false;
}

bool Xml_Parser::get_symbol(current* current,int len,wchar_t*delim,wstring* res){
	wchar_t*ch;
	int i,dlen;
	if(delim==NULL){
		delim=TEXT("");
	}
	while(current->n_curnt<len){
		ch=current->getcurrentstr();
		if(wcschr(delim,*ch)){
			return true;
		}
		res->append(1,*ch);
		
		current->inclement();
	}
	return false;
}

bool Xml_Parser::get_opera(current* current,int len,wchar_t*opera,wchar_t*blank,wchar_t*res){
	wchar_t*ch;
	while(current->n_curnt<len){
		ch=current->getcurrentstr();
		if(*res=*wcschr(opera,*ch)){//値
			return true;
		}
		if(!wcschr(blank,*ch)){
			break;
		}
		current->inclement();
	}
	return false;
}

bool Xml_Parser::split(current* current,opera_am* opera,int len,vector<wstring>*ress){
	wchar_t *pch;
	opera_am* curop=opera;
	map<wstring,opera_am*>::iterator itr;
	bool bres=false;
	while(current->n_curnt<len+10){
		pch=current->getcurrentstr();
		do{
			bres=false;
			for(itr=curop->next.begin();itr!=curop->next.end();itr++){
				if(wcsncmp(current->getcurrentstr(),itr->first.c_str(),itr->first.size())==0){
					current->inclement(itr->first.size());
					curop=itr->second;
					if(curop->next.empty()){
						return true;
					}
					if(ress->back()!=TEXT("")){
						ress->push_back(TEXT(""));
					}
					bres=true;
					break;
				}
			}
		}while(bres);
		//if(pch==current->getcurrentstr()){
			ress->back().append(1,*current->getcurrentstr());
			current->inclement();
		//}
	}
	return false;
}
//１つの要素の解析
int Xml_Parser::attribute(current* current,int len,Xml_DataPair* res){

	/*spcskip(current);
	wchar_t sp;
	if(!get_symbol(current,len,TEXT("\r\f\t\v\n ="),&res->key)){
		return false;
		//END
	}
	if(!namerule(res->key.c_str(),res->key.size(),NULL)){
		//ERRO
	}
	spcskip(current);
	if(*current->getcurrentstr()!='='){
		//ERROR
	}
	current->inclement();
	spcskip(current);
	sp=current->str[current->n_curnt];
	if(sp!='"'&&sp!='\''){
		//ERROR
	}
	current->inclement();
	if(!char_str(current,len,sp,&res->data)){
		//ERROR
	}
	//current->inclement();
	*/
	
	wchar_t prev;
	wstring *curnstr=NULL;
	wchar_t crn;

	get_symbol(current,len,TEXT("\r\t\f\v\n ="),&res->key);
	if(!namerule(res->key.c_str(),res->key.size(),NULL)){
		//ERROR
	}
	prev=' ';
	while(current->n_curnt<len){
		crn=*current->getcurrentstr();
		if(isspace(crn)){
			current->inclement();
		}else if(crn=='='){
			if(!isspace(prev)){
				//ERROR
			}
			prev=crn;
			current->inclement();
		}else if(crn=='"'||crn=='\''){
			if(prev=='='){
				prev=crn;
				while(1){
					crn=*current->getcurrentstr();
					if(crn=='<'){
						//ERROR
					}else if(crn==prev){
						break;
					}else if(crn=='&'){
						reference(current,ULONG_MAX,&res->data);
					}else{
						res->data.append(1,crn);
					}
					current->inclement();
				}
				//char_str(current,len,crn,&res->data);
				
				current->inclement();
				return true;
			}else{
				//ERROR
			}
		}else{
			//ERROR
		}
	}
	return false;

}

int Xml_Parser::element(current* current){

	int len;
	bool close;
	Xml_DataPair tmp;
	len=current->n_curnt+wcschr(current->getcurrentstr(),'>')-current->getcurrentstr();

	current->c_tree=current->c_tree->AddNode(new Tree());

	
	if(!get_symbol(current,len,TEXT("\r\t\f\v\n /"),&current->c_tree->elem)){
		//ERROR
	}
	if(!namerule(current->c_tree->elem.c_str(),current->c_tree->elem.size(),NULL)){
		//ERROR
	}

	wchar_t *ch;
	wchar_t prev;
	while(1){
		ch=current->getcurrentstr();
		if(isspace(*ch)){
			prev=' ';
		}else if(*ch=='/'&&*(ch+1)=='>'){
			current->inclement(2);
			current->c_tree=current->c_tree->GetParent();
			break;
		}else if(*ch=='>'){
			current->inclement();
			break;
		}else{
			if(prev==' '){
				current->c_tree->Attrib.push_back(tmp);
				attribute(current,len,&current->c_tree->Attrib.back());
				prev=NULL;
			}else{
				//ERROR
			}
		}
		current->inclement();
	}
	bifurc(current);
	
	return true;
}


bool Xml_Parser::bifurc(current* current){
	int len;

	if(*current->getcurrentstr()!='<'){
		if(current->c_tree->GetParent()==NULL){
			return false;
		}
		//再帰するとスタックオーバーフロー
		while(*current->getcurrentstr()!='<'){
			if(*current->getcurrentstr()=='&'){
				reference(current,ULONG_MAX,&current->c_tree->str);
			}else{
				current->c_tree->str.append(1,*current->getcurrentstr());
			}
			current->inclement();
		}
		bifurc(current);
		return true;
	}
	current->inclement();
	switch(*current->getcurrentstr()){
	case '!':
		current->inclement();
		if(wcsncmp(current->getcurrentstr(),TEXT("--"),2)==0){
			current->inclement(wcsstr(current->getcurrentstr(),TEXT("!-->"))-current->getcurrentstr()+lstrlen(TEXT("!-->")));
		}else if(wcsncmp(current->getcurrentstr(),TEXT("[CDATA["),lstrlen(TEXT("[CDATA[")))==0){
			current->inclement(lstrlen(TEXT("[CDATA[")));
			current->c_tree->str.append(current->getcurrentstr(),wcsstr(current->getcurrentstr(),TEXT("]]>"))-current->getcurrentstr());
			current->inclement(lstrlen(TEXT("]]>")));
		}
		break;
	case '?':
		current->inclement(wcschr(current->getcurrentstr(),'>')-current->getcurrentstr()+1);
		spcskip(current);
		bifurc(current);
		//?;
		break;
	case '/':
		{
			current->inclement();
			wstring res;
			get_symbol(current,ULONG_MAX,TEXT(">"),&res);
			if(current->c_tree->elem!=res){
				//ERROR
			}
			current->inclement();
			current->c_tree=current->c_tree->GetParent();
			if(current->c_tree->GetParent()!=NULL){
				spcskip(current);
				bifurc(current);
			}
		}
		break;
	default:
		element(current);
		break;
	}
	return true;
}
//rev2
void Xml_Parser::Parse(wchar_t* xml){
	struct current current;
	current.Init(xml);
	spcskip(&current);
	bifurc(&current);
}