#ifndef _H_BASE64_H_
#define	_H_BASE64_H_

#define BASE64_ENCODE_SUCCEEDED           0	
#define BASE64_DECODE_SUCCEEDED           0
#define BASE64_GENERAL_ERROR              1
#define BASE64_ENCODE_BUFFER_NONE         2
#define BASE64_MEMORY_NOT_ENOUGH          3
#define BASE64_OUTBUF_SIZE_TOO_SMALL      4
#define BASE64_DECODE_BUFFER_NONE         5
#define BASE64_DECODE_STRING_NONE         6
#define BASE64_DECODE_STR_LENERR          7 /*字符串必须是4的倍数*/

#ifdef __cplusplus
extern "C" {
#endif

/** 
 *   @brief                          对二进制输入进行base64编码
 *                                   输出长度是输入长度(补齐3字节之后)的 4 / 3 倍
 *   @param  [in]   in_buf           输入缓冲区
 *   @param  [in]   in_buf_len       输入数据长度
 *   @param  [out]  out_buf          输出缓冲区
 *   @param  [in]   out_buf_max_len  输出缓冲区大小，防止溢出
 *   @param  [out]  out_buf_len      编码后的二进制长度
 *   @return  如成功返回BASE64_ENCODE_SUCCEEDED, 如失败返回错误码
 */
int base64_encode (
    char  *in_buf,
    int   in_buf_len,
    char  *out_buf,
    int   out_buf_max_len,
    int   *out_buf_len
);

/** 
 *   @brief                          对base64编码的数据进行解码
 *   @param  [in]   in_buf           输入缓冲区
 *   @param  [in]   in_buf_len       输入数据长度
 *   @param  [out]  out_buf          输出缓冲区
 *   @param  [in]   out_buf_max_len  输出缓冲区大小，防止溢出
 *   @param  [out]  out_buf_len      编码后的二进制长度
 *   @return  如成功返回BASE64_DECODE_SUCCEEDED, 如失败返回错误码
 */
int base64_decode (
    char  *in_buf,
    int   in_buf_len,
    char  *out_buf,
    int   out_buf_max_len,
    int   *out_buf_len
);

#ifdef __cplusplus
}
#endif
	
#endif