# 智能门禁　算法库  

## 概述:  
  这个算法库主要包含智能门禁系统中与WebService　接口对接时候用到的算法库,主要包括md5, base64及3DES-ECB
  


## Test:   
	make run:  
	
	
		[Base64 Encode/Decode Test]:
		origin  ---        < [31] [32] [33] [34] [35] [36] [37]  >
		coded   ---        < MTIzNDU2Nw== >
		decoded ---        < [31] [32] [33] [34] [35] [36] [37]  >

		[MD5 Encode Test]:
		string  ---         < hello >
		coded   ---         < [5D] [41] [40] [2A] [BC] [4B] [2A] [76] [B9] [71] [9D] [91] [10] [17] [C5] [92]  >

		[3Des-Ecb Encode/DeCode Test]:
		origin  ---         < 01234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQ >
		coded   ---         < /s4o9YYYsQqFTREnMm07x02KAiOwHrczObP71fm4AQKzP2GpjsNqxoRgUFBFRaEZ0ykY5REK5Ik= >
		used time: 1115 us
		decoded ---         < 01234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQ >
		used time: 739 us
  
## Md5:  
	* 加密:  
  const char \*str = "hello";  
  char out[16];  
  algo\_Md5Encode(str, strlen(str), out);  


## Base64:  
	* 编码:  
    char buf[] = {'1', '2', '3', '4', '5', '6', '7'};  
    int len = algo_Base64EncodeLen(sizeof(buf));  
    char *out = malloc(len);  
    algo_Base64Encode(out, buf, sizeof(buf));  
    free(out);  

	* 解码:  
    int dlen = algo_Base64DecodeLen(out);  
    char *dout = malloc(dlen);  
    algo_Base64Decode(dout, out);  
    free(dout);  
    
  
## 3DES-ECB: 
	* 加密:  
	char *origin = "hello";  
	char enstr[128];  
	algo_3des_ecb_init("0123456789abcdef0123456789abcdef0123456789abcdef");  
	algo_3des_ecb_encrypt(origin, enstr);  

	* 解密:  
	char destr[128];  
	algo_3des_ecb_decrypt(enstr, destr);  

	

