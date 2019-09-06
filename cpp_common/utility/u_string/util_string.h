
#ifndef _UTIL_STRING_H_
#define _UTIL_STRING_H_


#include <string>
#include <vector>

namespace stringutil
{
    // 字节与16进制字符串相互转换，可指定输出的大小写
    std::string BinToHexstr(const std::string& bin, bool upper);
    std::string HexstrToBin(const std::string& hexstr);
    
	// 字符串按分隔符拆分
    std::vector<std::string> Split(const std::string& str, const std::string& separator);
    std::vector<std::string> Split(const std::string& str, char separator);

	// trim
    void LeftTrim(std::string& str);
    void RightTrim(std::string& str);
    void Trim(std::string& str);
    
	// 字符串替换
    void ReplaceAll(std::string& str, const std::string& target, const std::string& replacement);
    
	// 大小写转换
    void ToLower(std::string& str);
    void ToUpper(std::string& str);
    
	// 字节数组与base64转换
    std::string Base64Encode(const std::string& src);
    std::string Base64Decode(const std::string& base64);
    
    std::string Md5OfBlock(const string& content);
    std::string Md5OfFile(const string& filepath);
};









#endif // _UTIL_STRING_H_
