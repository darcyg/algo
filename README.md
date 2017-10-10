# 智能门禁　算法库  

## 概述:  
  这个算法库主要包含智能门禁系统中与WebService　接口对接时候用到的算法库,主要包括md5, base64及3DES-ECB
  
  
## md5:  
  * 加密:  
  const char *str = "hello";  
  char out[16];  
  algo_Md5Encode(str, strlen(str), out);  
  
  
## base64:  
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
