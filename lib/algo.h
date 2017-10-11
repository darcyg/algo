#ifndef __ALGO_H_
#define __ALGO_H_


#ifdef __cplusplus 
extern "C" {
#endif


/* base64 */

/* algo_Base64EncodeLen
 * 
 * 计算 len 长度字节 base64 编码后字节长度 
 *
 * @param[in] len 需要编码的字节长度 
 * 
 * @return 返回计算编码后字节长度
 */
int algo_Base64EncodeLen(int len);


/* algo_Base64Encode 
 *
 * base64 编码
 *
 * @param[in] encoded 编码后字符
 * @param[in] string 需要编码的数据
 * @param[in] len 需要编码的数据的长度
 *
 * @return 返回实际编码后数据的长度
 */
int algo_Base64Encode(char *encoded, const char *string, int len);


/* algo_Base64DecodeLen 
 * 
 * 计算需要解码的字节的长度, base64后面有多个'='是不需要解码的
 * 
 * @param[in] bufcoded 需要解码的字符串
 * 
 * @return 返回计算需要解码的字节长度
 */
int algo_Base64DecodeLen(const char *bufcoded);


/* algo_Base64Decode 
 * 
 * base64 解码
 *
 * @param[out] bufplain 解码后的数据
 * @param[in] bufcoded 需要被解码的字符串
 * 
 * @return 返回解码后数据的长度
 */
int algo_Base64Decode(char *bufplain, const char *bufcoded);



/* md5 */
/* algo_Md5Encode
 * 
 * md5加密
 * 
 * @param[in] bufplain 被加密的数据
 * @param[in] len 数据的长度
 * @param[out] bufcoded md5加密后的数据
 *
 * @return 返回0
 */
int algo_Md5Encode(const char *bufplain, int len, char bufcoded[16]);



/* 3des ecb */
	void algo_3des_ecb_init(char *key);
	int algo_3des_ecb_encrypt(char *str, char *enstr);
	int algo_3des_ecb_decrypt(char *str, char *destr);
	void algo_3des_ecb_free();

#ifdef __cplusplus
}
#endif

#endif
