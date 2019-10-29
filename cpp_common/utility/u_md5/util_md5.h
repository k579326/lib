#ifndef _UTIL_MD5_H_
#define _UTIL_MD5_H_


#include <string>

namespace md5util
{
    
    std::string Md5OfBlock(const std::string& content, bool upper);
    std::string Md5OfFile(const std::string& filepath, bool upper);
 
};























#endif // _UTIL_MD5_H_

