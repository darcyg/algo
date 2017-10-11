#include "algo.h"

#include "md5.h"
#include "base64.h"
#include "des.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
static Des	*des = NULL;

void algo_3des_ecb_init(char *key) {
	if (des != NULL) {
		delete des;
		des = NULL;
	}

	des = new Des(key);
}

int algo_3des_ecb_encrypt(char *str, char *enstr) {
	if (des == NULL) {
		return -1;
	}
		
	std::string es = des->desEncrypt(str);
	
	if (es.compare(nullptr) == 0) {
		return -2;
	}

	strcpy(enstr, es.c_str());

	return strlen(enstr);
}

int algo_3des_ecb_decrypt(char *str, char *destr) {
	if (des == NULL) {
		return -1;
	}

	std::string ds = des->desDecrypt(str);

	if (ds.compare(nullptr) == 0) {
		return -2;
	}

	strcpy(destr, ds.c_str());

	return strlen(destr);
}

void algo_3des_ecb_free() {
	delete des;
	des = NULL;
}

