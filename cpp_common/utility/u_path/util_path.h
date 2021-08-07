#ifndef _UTIL_PATH_H_
#define _UTIL_PATH_H_

#include <string>

namespace pathutil
{
    enum class PathStyle : char
    {
        kWindows,
        kUnix,
        kAuto
    };
    
    // 风格转换（unix与windows路径风格自适应），并移除重复的斜杠
    void PathStyleConvert(std::string& path, PathStyle to_style = PathStyle::kAuto);
    
    // 路径拼接
    std::string PathCombines(const std::string& dir, const std::string& filename);
    
    // 获取父目录
    std::string GetDirOfPathName(const std::string& path);
    
    std::string GetShortNameOfPath(const std::string& path);

    std::string RelativeToAbsolute(const std::string& reltpath);
};













#endif // _UTIL_PATH_H_
