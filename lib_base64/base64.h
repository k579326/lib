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
#define BASE64_DECODE_STR_LENERR          7 /*�ַ���������4�ı���*/

#ifdef __cplusplus
extern "C" {
#endif

/** 
 *   @brief                          �Զ������������base64����
 *                                   ������������볤��(����3�ֽ�֮��)�� 4 / 3 ��
 *   @param  [in]   in_buf           ���뻺����
 *   @param  [in]   in_buf_len       �������ݳ���
 *   @param  [out]  out_buf          ���������
 *   @param  [in]   out_buf_max_len  �����������С����ֹ���
 *   @param  [out]  out_buf_len      �����Ķ����Ƴ���
 *   @return  ��ɹ�����BASE64_ENCODE_SUCCEEDED, ��ʧ�ܷ��ش�����
 */
int base64_encode (
    char  *in_buf,
    int   in_buf_len,
    char  *out_buf,
    int   out_buf_max_len,
    int   *out_buf_len
);

/** 
 *   @brief                          ��base64��������ݽ��н���
 *   @param  [in]   in_buf           ���뻺����
 *   @param  [in]   in_buf_len       �������ݳ���
 *   @param  [out]  out_buf          ���������
 *   @param  [in]   out_buf_max_len  �����������С����ֹ���
 *   @param  [out]  out_buf_len      �����Ķ����Ƴ���
 *   @return  ��ɹ�����BASE64_DECODE_SUCCEEDED, ��ʧ�ܷ��ش�����
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