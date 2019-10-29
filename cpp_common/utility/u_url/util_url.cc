#include "util_url.h"





namespace urlutil
{
    
bool UrlAppendParam(std::string& url, const std::string& key, const std::string& value)
{
    size_t pos = url.rfind('?');

    std::string pair = key + "=" + value;
    if (pos != std::string::npos) {
        url += ("&" + pair);
    }
    else
    {
        url += ("?" + pair);
    }
    
    return true;
}
bool UrlRemoveParam(std::string& url, const std::string& key)
{
    return false;
}

bool UrlUpdateParam(std::string& url, const std::string& key, const std::string& value)
{
    return false;
}

};


