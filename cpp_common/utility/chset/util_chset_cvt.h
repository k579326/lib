#ifndef _UTIL_CHSET_CVT_H_
#define _UTIL_CHSET_CVT_H_

#include <string>


namespace chsetutil
{
    
    // string的编码方式为utf8，则采用：
    std::string WideStringToUTF8(const std::wstring& str);

    std::wstring UTF8ToWideString(const std::string& str);

    // string的编码方式为除utf8外的其它编码方式，可采用：
    std::string WideStringToLocal(const std::wstring& str, const std::string& locale);  //locale = "chs"或"zh-cn"
    
    std::wstring LocalToWideString(const std::string& str, const std::string& locale);
    
    
    
};






#endif

