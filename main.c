#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "algo.h"

void view_buf(char *buf, int len);
void base64_test();
void md5_test();
void triple_des_ecb_test();

int main(int argc, char *argv[]) {

	/* base64 test */
	base64_test();


	/* md5 test */
	md5_test();

	/* 3des ecb */
	triple_des_ecb_test();

	return 0;
}

void base64_test() {
	//char buf[] = {0x01, 0x02, 0x03, 0x04, 0x05};
	char buf[] = {'1', '2', '3', '4', '5', '6', '7'};
	int len = algo_Base64EncodeLen(sizeof(buf));
	char *out = (char *)malloc(len);
	algo_Base64Encode(out, buf, sizeof(buf));

	int dlen = algo_Base64DecodeLen(out);
	char *dout = (char *)malloc(dlen);
	algo_Base64Decode(dout, out);
	
	printf("\n[Base64 Encode/Decode Test]:\n");
	printf("origin  ---        < ");
	view_buf(buf, sizeof(buf));
	printf(" >\n");

	printf("coded   ---        < ");
	printf("%s", out);
	printf(" >\n");

	printf("decoded ---        < ");
	view_buf(dout, dlen);
	printf(" >\n");


	free(out);
	free(dout);
}

void md5_test() {
	const char *str = "hello";
	char out[16];
	
	algo_Md5Encode(str, strlen(str), out);
	
	
	printf("\n[MD5 Encode Test]:\n");
	printf("string  ---         < ");
	printf("%s", str);
	printf(" >\n");
	printf("coded   ---         < ");
	view_buf(out, sizeof(out));
	printf(" >\n");
	
}


void triple_des_ecb_test() {
	char *origin = "hello";
	char enstr[128];
	char destr[128];

	algo_3des_ecb_init("0123456789abcdef0123456789abcdef0123456789abcdef");
	
	algo_3des_ecb_encrypt(origin, enstr);

	algo_3des_ecb_decrypt(enstr, destr);
	
	printf("\n[3Des-Ecb Encode/DeCode Test]:\n");
	printf("origin  ---         < ");
	printf("%s", origin);
	printf(" >\n");
	printf("coded   ---         < ");
	printf("%s", enstr);
	printf(" >\n");
	printf("decoded ---         < ");
	printf("%s", destr);
	printf(" >\n");
	
}

void view_buf(char *buf, int len) {
	int i = 0;
	for (i = 0; i < len; i++) {
		printf("[%02X] ", buf[i]&0xff);
		if ( (i + 1) % 20 == 0) {
			printf("\n");
		}
	}
}

