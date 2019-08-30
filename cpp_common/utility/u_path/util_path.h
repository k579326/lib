#ifndef _UTIL_PATH_H_
#define _UTIL_PATH_H_

#include <string>

namespace pathutil
{
    enum PathStyle
    {
        kWindows,
        kUnix,
        kAuto
    };
    
    // 风格转换（unix与windows路径风格自适应）
    void PathStyleConvert(std::string& path, PathStyle to_style);
    
    // 路径拼接
    std::string PathCombines(const std::string& dir, const std::string& filename);
    
    // 获取父目录
    std::string GetDirOfPathName(const std::string& path);
    
};













#endif // _UTIL_PATH_H_
