#ifndef HMAC_SHA1_
#define HMAC_SHA1_

#include <stdint.h>
#include <string.h>
#include <string>
using namespace std;
//認証用

/*↓ここに載ってるのをパクった↓
http://www.ipa.go.jp/security/rfc/RFC3174JA.html#072
  ↑ここに載ってるのをパクった↑*/
#define SHA1_HASH_SIZE 20
//↓関数化しました
//#define Sha1CircularShift(bits,word) (((word)<<(bits))|((word)>>(32-(bits))))

enum{shaSuccess=0,shaNull,shaInputTooLong,shaStateError};


class Sha1{
	struct sha1_ctx{
		uint32_t Hash[SHA1_HASH_SIZE/4];
		uint32_t Length_Low;
		uint32_t Length_High;

		int_least16_t Message_Block_Index;
		uint8_t Message_Block[64];//512bit
		int Computed;
		int Corrupted;
	};
	sha1_ctx sha1;
	uint32_t Sha1CircularShift(uint32_t bits,uint32_t word){
		return (word<<bits)|(word>>(32-bits));
	}
	void Sha1ProcessMessageBlock();
	void Sha1PadMessage();
public:
	Sha1();
	void Init();
	int GetResult(uint8_t[SHA1_HASH_SIZE]);
	int GetResult_Hex(string*);
	int GetResult_Base64(string*);
	int Input(const uint8_t*,unsigned int);
};

class Base64{
private:
	char convert(unsigned char);
public:
	bool Encode(unsigned char*,int,string*);
};


class HMac_Sha1{
public:
	bool Encode(char*,char*,string*);
};

#endif