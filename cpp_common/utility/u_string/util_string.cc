

#include "util_string.h"

#include <cctype>
#include <algorithm>

namespace stringutil
{
    static char _HexchOfNum(uint8_t num, bool upper)
    {
        if (num >= 16) {
            return -1;
        }

        if (num >= 0 && num < 10) {
            return num + '0';
        }
        else
        {
            return num - 10 + (upper ? 'A' : 'a');
        }
    }

    static bool _IsValidHexch(char ch)
    {
        return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
    }


    // hbits 字节高4位，lbits 字节低4位
    static char _NumOfHexch(uint8_t hbits, uint8_t lbits)
    {
        if (hbits >= 'a' && hbits <= 'f') {
            hbits = hbits - 'a' + 10;
        }
        if (hbits >= 'A' && hbits <= 'F') {
            hbits = hbits - 'A' + 10;
        }
        if (hbits >= '0' && hbits <= '9') {
            hbits = hbits - '0';
        }


        if (lbits >= 'a' && lbits <= 'f') {
            lbits = lbits - 'a' + 10;
        }
        if (lbits >= 'A' && lbits <= 'F') {
            lbits = lbits - 'A' + 10;
        }
        if (lbits >= '0' && lbits <= '9') {
            lbits = lbits - '0';
        }

        return (hbits << 4) + lbits;
    }



    // 字节与16进制字符串相互转换
    std::string BinToHexstr(const std::string& bin, bool upper)
    {
        std::string x;
        
        for (auto& ch : bin)
        {
            x.append(1, _HexchOfNum((uint8_t)ch / 16, upper));
            x.append(1, _HexchOfNum((uint8_t)ch % 16, upper));
        }
        
        return x;
    }
    std::string HexstrToBin(const std::string& hexstr)
    {
        std::string x, ava;

        ava = hexstr;
        ToLower(ava);

        if (ava.size() % 2 != 0) {
            return "";
        }

        for (int i = 0; i < ava.size(); i = i + 2)
        {
            if (!_IsValidHexch(ava[i]) || !_IsValidHexch(ava[i + 1])) {
                return "";
            }

            x.append(1, _NumOfHexch(ava[i], ava[i + 1]));
        }

        return x;
    }
    
	// 字符串按分隔符拆分
    std::vector<std::string> Split(const std::string& str, const std::string& separator)
    {
        std::vector<std::string> container;
        size_t s_pos = 0, e_pos = 0;

        if (str.empty()) {
            return std::vector<std::string>();
        }

        while (true)
        {
            e_pos = str.find(separator, s_pos);
            if (e_pos == std::string::npos)
            {
                break;
            }

            container.push_back(str.substr(s_pos, e_pos - s_pos));
            s_pos = e_pos + separator.size();
        }

        container.push_back(str.substr(s_pos, std::string::npos));
        return container;
    }
    
    std::vector<std::string> Split(const std::string& str, char separator)
    {
        std::string x(1, separator);
        return Split(str, x);
    }

	// trim
    void LeftTrim(std::string& str)
    {
        str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](char e)->bool { return !std::isspace(e); }));
    }
    void RightTrim(std::string& str)
    {
        str.erase(std::find_if(str.rbegin(), str.rend(), [](char e)->bool { return !std::isspace(e); }).base(), str.end());
    }
    void Trim(std::string& str)
    {
        LeftTrim(str);
        RightTrim(str);
    }
    
	// 字符串替换
    void ReplaceAll(std::string& str, const std::string& target, const std::string& replacement)
    {
        size_t off = 0, t_pos = 0;
        while (off != std::string::npos)
        {
            t_pos = str.find(target, off);
            if (t_pos == std::string::npos) {
                break;
            }

            str.replace(t_pos, target.size(), replacement.c_str());
            off = t_pos + replacement.size();
        }

        return;
    }
    
    bool IsNumber(const std::string& str)
    {
        // 处理空串
        if (str.empty()) {
            return false;
        }

        // 先将这个字符串 "-" 处理掉
        if (str[0] == '-' && str.size() == 1) {
            return false;
        }

        for (int i = 0; i < str.size(); i++)
        {
            if (i == 0 && str[i] == '-') {
                continue;
            }

            if (str[i] > '9' || str[i] < '0') {
                return false;
            }
        }

        return true;
    }



	// 大小写转换
    void ToLower(std::string& str)
    {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    }
    void ToUpper(std::string& str)
    {
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    }
    
	// 字节数组与base64转换
    std::string Base64Encode(const std::string& src)
    {
        return "";
    }
    std::string Base64Decode(const std::string& base64)
    {
        return "";
    }
   
};











