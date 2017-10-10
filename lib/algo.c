#include "algo.h"

#include "md5.h"
#include "base64.h"

/* base64 */
int algo_Base64EncodeLen(int len) {
	return Base64encode_len(len);
}

int algo_Base64Encode(char *encoded, const char *string, int len) {
	return Base64encode(encoded, string, len);
}

int algo_Base64DecodeLen(const char *bufcoded) {
	return Base64decode_len(bufcoded)*6/8;
}

int algo_Base64Decode(char *bufplain, const char *bufcoded) {
	return Base64decode(bufplain, bufcoded);
}


/* md5 */
int algo_Md5Encode(const char *bufplain, int len, char bufcoded[16]) {
	unsigned char *encrypt = (unsigned char *)bufplain;
	unsigned char *decrypt = (unsigned char *)bufcoded;

	MD5_CTX md5;  

	MD5Init(&md5);                

	MD5Update(&md5,encrypt,len);  

	MD5Final(decrypt, &md5);          

	/* 
	printf("md5 before: %s\n md5 after:",encrypt);  
	int i = 0;
	for (i=0; i<16; i++) {  
		printf("%02x",decrypt[i]);  
	}  
	*/
	return 0;
}


/* 3DES ECB */
