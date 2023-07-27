
#ifndef _UTIL_STRING_H_
#define _UTIL_STRING_H_


#include <string>
#include <vector>

namespace stringutil
{
    typedef struct _suxx_guid {
        unsigned int   Data1;
        unsigned short Data2;
        unsigned short Data3;
        unsigned char  Data4[8];
    } GUID;

    // 字节与16进制字符串相互转换，可指定输出的大小写
    std::string BinToHexstr(const std::string& bin, bool upper);
    std::string HexstrToBin(const std::string& hexstr);
    
	// trim
    void LeftTrim(std::string& str);
    void RightTrim(std::string& str);
    void Trim(std::string& str);
    
	// 字符串替换
    void ReplaceAll(std::string& str, const std::string& target, const std::string& replacement);
    
    bool IsNumber(const std::string& str);

	// 大小写转换
    void ToLower(std::string& str);
    void ToUpper(std::string& str);
    
	// 字节数组与base64转换
    std::string Base64Encode(const std::string& src);
    std::string Base64Decode(const std::string& base64);
    

    std::string GuidToString(const stringutil::GUID& guid, bool to_upper);
    stringutil::GUID StringToGuid(const std::string& guid_string);


    // 字符串按分隔符拆分
    template<class T>
    std::vector<T> Split(const T& str, const T& separator)
    {
        std::vector<T> container;
        size_t s_pos = 0, e_pos = 0;

        if (str.empty()) {
            return std::vector<T>();
        }

        while (true)
        {
            e_pos = str.find(separator, s_pos);
            if (e_pos == std::string::npos)
            {
                break;
            }
            if (e_pos != s_pos) {
                container.push_back(str.substr(s_pos, e_pos - s_pos));
            }
            s_pos = e_pos + separator.size();
        }

        container.push_back(str.substr(s_pos, std::string::npos));
        return container;
    }

    template<class T>
    std::vector<T> Split(const T& str, typename T::value_type separator)
    {
        T x(1, separator);
        return Split<T>(str, x);
    }


    template<class T>
    T EndianConvert(T t)
    {
        char* __p = reinterpret_cast<char*>(&t);
        char* __ep = __p + sizeof(T) - 1;

        for (int i = 0; i < sizeof(T) / 2; i++) {
            std::swap(*(__p + i), *(__ep - i));
        }
        return t;
    }

};









#endif // _UTIL_STRING_H_
