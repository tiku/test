#include "HMAC_SHA1.h"

/*
***********************************
***             Sha1            ***
***********************************
*/


Sha1::Sha1(){
	Init();
}

void Sha1::Init(){
	sha1.Hash[0]=0x67452301;
	sha1.Hash[1]=0xEFCDAB89;
	sha1.Hash[2]=0x98BADCFE;
	sha1.Hash[3]=0x10325476;
	sha1.Hash[4]=0xC3D2E1F0;

	sha1.Length_Low=0;
	sha1.Length_High=0;
	sha1.Message_Block_Index=0;
	sha1.Computed=0;
	sha1.Corrupted=0;
}

int Sha1::GetResult(uint8_t MsgDigest[SHA1_HASH_SIZE]){
	int i;
	if(!MsgDigest){
		return shaNull;
	}
	if(sha1.Corrupted){
		return sha1.Corrupted;
	}
	//memset(&MsgDigest,0,sizeof(MsgDigest));
	if(!sha1.Computed){
		Sha1PadMessage();
		for(i=0;i<64;i++){
			sha1.Message_Block[i]=0;
		}
		sha1.Length_Low=0;
		sha1.Length_High=0;
		sha1.Computed=1;
	}

	for(i=0;i<SHA1_HASH_SIZE;i++){
		MsgDigest[i]=sha1.Hash[i>>2]>>8*(3-(i&0x03));
	}
	return shaSuccess;
}
//実験
int Sha1::GetResult_Hex(string* res){
	int i;
	uint8_t digest[20];
	char ch[3];
	res->clear();
	GetResult(digest);
	for(i=0;i<20;i++){
		sprintf(ch,"%X",digest[i]);
		res->append(ch);
	}
	return true;
}
int Sha1::GetResult_Base64(string*res){
	Base64 b64;
	uint8_t digest[21];
	GetResult(digest);

	b64.Encode(digest,20,res);
	return true;
}

int Sha1::Input(const uint8_t*msgarr,unsigned int length){
	if(!length){
		shaSuccess;
	}
	if(!msgarr){
		shaNull;
	}

	if(sha1.Computed){
		sha1.Corrupted=shaStateError;
		return shaStateError;
	}
	while(length--&&!sha1.Corrupted){
		sha1.Message_Block[sha1.Message_Block_Index++]=(*msgarr&0xff);
		sha1.Length_Low+=8;
		if(sha1.Length_Low==0){
			sha1.Length_High++;
			if(sha1.Length_High==0){
				sha1.Corrupted=1;
			}
		}
		if(sha1.Message_Block_Index==64){
			Sha1ProcessMessageBlock();
		}
		msgarr++;
	}
	return shaSuccess;
}

void Sha1::Sha1ProcessMessageBlock(){
	const uint32_t K[]={0x5A827999, 0x6ED9EBA1,0x8F1BBCDC,0xCA62C1D6};
	int t;
	uint32_t tmp;
	uint32_t W[80];
	uint32_t A,B,C,D,E;

	for(t=0;t<16;t++){
		W[t]=sha1.Message_Block[t*4]<<24;
		W[t]|=sha1.Message_Block[t*4+1]<<16;
		W[t]|=sha1.Message_Block[t*4+2]<<8;
		W[t]|=sha1.Message_Block[t*4+3];
	}
	for(t=16;t<80;t++){
		W[t]=Sha1CircularShift(1,W[t-3]^W[t-8]^W[t-14]^W[t-16]);
	}

	A=sha1.Hash[0];
	B=sha1.Hash[1];
	C=sha1.Hash[2];
	D=sha1.Hash[3];
	E=sha1.Hash[4];

	for(t=0;;t++){
		if(t<20){
			tmp=Sha1CircularShift(5,A)+((B&C)|((~B)&D))+E+W[t]+K[0];
		}else if(t<40){
			tmp=Sha1CircularShift(5,A)+(B^C^D)+E+W[t]+K[1];
		}else if(t<60){
			tmp=Sha1CircularShift(5,A)+((B&C)|(B&D)|(C&D))+E+W[t]+K[2];
		}else if(t<80){
			tmp=Sha1CircularShift(5,A)+(B^C^D)+E+W[t]+K[3];
		}else{
			break;
		}
		E=D;
		D=C;
		C=Sha1CircularShift(30,B);
		B=A;
		A=tmp;
	}

	sha1.Hash[0]+=A;
	sha1.Hash[1]+=B;
	sha1.Hash[2]+=C;
	sha1.Hash[3]+=D;
	sha1.Hash[4]+=E;

	sha1.Message_Block_Index=0;
	return;
}

void Sha1::Sha1PadMessage(){
	if(sha1.Message_Block_Index>55){
		sha1.Message_Block[sha1.Message_Block_Index++]=0x80;
		while(sha1.Message_Block_Index<64){
			sha1.Message_Block[sha1.Message_Block_Index++]=0;
		}
		Sha1ProcessMessageBlock();
		while(sha1.Message_Block_Index<56){
			sha1.Message_Block[sha1.Message_Block_Index++]=0;
		}
	}else{
		sha1.Message_Block[sha1.Message_Block_Index++]=0x80;
		while(sha1.Message_Block_Index<56){
			sha1.Message_Block[sha1.Message_Block_Index++]=0;
		}
	}
	sha1.Message_Block[56]=sha1.Length_High>>24;
	sha1.Message_Block[57]=sha1.Length_High>>16;
	sha1.Message_Block[58]=sha1.Length_High>>8;
	sha1.Message_Block[59]=sha1.Length_High;
	sha1.Message_Block[60]=sha1.Length_Low>>24;
	sha1.Message_Block[61]=sha1.Length_Low>>16;
	sha1.Message_Block[62]=sha1.Length_Low>>8;
	sha1.Message_Block[63]=sha1.Length_Low;

	Sha1ProcessMessageBlock();
	return;
}

/*
***********************************
***           Base64            ***
***********************************
*/

char Base64::convert(unsigned char ch){
	if(ch<=0x19){//0x19=0001-1001=25
		return ch+'A';
	}else if(ch<=0x33){//0x33=0011-0011=51
		return (ch-0x1A)+'a';//0x20=0001-1010=26
	}else if(ch<=0x3D){//0x3D=0011-1101=61
		return (ch-0x34)+'0';//0x3c=0011-0100=52
	}else if(ch==0x3E){//0x3E=0011-1110=62
		return '+';//これしか無いのでダイレクトに
	}else if(ch==0x3F){//0x3F=0011-1111=63
		return '/';//同上
	}else{
		return '=';
	}
	return NULL;
}

//Unsignedで受け取ります(Unsignedがないと、シフト演算が算術シフトになってバグる。後全角がマイナスの値になってしまう）
bool Base64::Encode(unsigned char* src,int len,string*res){
	const int charbit=8;
	string dest;
	int i,sfi;
	unsigned char prev=0,ch;//UnSignedにしないとバグります(マイナスの値になるので)
	src[len]=0;//最後にNULL文字を挿入
	i=0;
	sfi=2;
	while(i<=len){
		ch=src[i];
		ch=(src[i]>>(sfi))|(prev);
		dest.append(1,convert(ch));
		prev=src[i]<<(charbit-sfi);//2つ余分にシフトする（邪魔な上位2bitを0で埋めるため）；
		prev=prev>>2;//余分にシフトした分を戻す
		if(sfi==charbit){
			i--;
			sfi=0;
		}
		i++;
		sfi+=2;
	}
	if(dest.size()%4!=0){
		dest.append(4-dest.size()%4,'=');
	}
	*res=dest;
	return true;
}


/*
***********************************
***          HMAC-SHA1          ***
***********************************
*/


bool HMac_Sha1::Encode(char* key,char* msg,string* res){
	const int max_key=64,max_digest=20;
	char cx_ipad=0x36,cx_opad=0x5c;
	Sha1 sha1;
	int keylen,i;
	uint8_t c_key[max_key];
	uint8_t c_opad[max_key],c_ipad[max_key],c_ipad_res[max_digest],c_res[max_digest];
	if(64<strlen(key)){
		sha1.Init();
		sha1.Input((unsigned char*)key,strlen(key));
		sha1.GetResult(c_key);
		keylen=max_digest;
	}else{
		keylen=strlen(key);
		strcpy((char*)c_key,key);
	}
	sha1.Init();
	memset(c_opad,cx_opad,sizeof(c_opad));
	memset(c_ipad,cx_ipad,sizeof(c_ipad));
	for(i=0;i<keylen;i++){
		c_opad[i]=c_key[i]^cx_opad;
		c_ipad[i]=c_key[i]^cx_ipad;
	}
	sha1.Input(c_ipad,max_key);
	sha1.Input((unsigned char*)msg,strlen(msg));
	sha1.GetResult(c_ipad_res);
	sha1.Init();
	sha1.Input(c_opad,max_key);
	sha1.Input(c_ipad_res,max_digest);
	sha1.GetResult_Base64(res);

	//途中に0が入っているとNULL文字と認識されてコピーされないのでmemcpyで
	//memcpy(res,c_res,20);
	return false;
}
