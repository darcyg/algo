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
/* algo_3des_ecb_init
 * 
 * 3des ecb 加密初始化
 * 
 * @param key[in] 加密密匙, 32字节或者48字节长度, 例如密码是
 * "111111111111111111111111", 24个1,这里应该填写
 * "31313131313131313131313131313131313131313131313131313131", 就是1的hex string形式
 * 
 * @return none
 */
void algo_3des_ecb_init(char *key);


/* algo_3des_ecb_encrypt
 * 
 * 3des ecb 加密
 * 
 * @param str[in] 需要加密的字符串, 原始字符串
 * @param enstr[out] 加密输出的字符串,经过base64 编码
 *
 * @return 返回加密后字符串长度
 */
int algo_3des_ecb_encrypt(char *str, char *enstr);

/* algo_3des_ecb_decrypt
 * 
 * 3des ecb 解密
 * 
 * @param str[in] 需要解密的字符串, 经过base64的
 * @param destr[out] 解密后的字符串,
 *
 * @return 返回　解密字符串长度 
 */
int algo_3des_ecb_decrypt(char *str, char *destr);



/* algo_3des_ecb_free
 * 
 * 3des ecb 释放
 * 
 * @param none
 *
 * @return none
 */
void algo_3des_ecb_free();


#ifdef __cplusplus
}
#endif

#endif
