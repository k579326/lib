#ifndef _UTIL_URL_H_
#define _UTIL_URL_H_


#include <string>



namespace urlutil
{
    
bool UrlAppendParam(std::string& url, const std::string& key, const std::string& value);
bool UrlRemoveParam(std::string& url, const std::string& key);
bool UrlUpdateParam(std::string& url, const std::string& key, const std::string& value);

};




























#endif // _UTIL_URL_H_


