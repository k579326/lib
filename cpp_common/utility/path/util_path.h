#ifndef _UTIL_PATH_H_
#define _UTIL_PATH_H_

#include <string>
#include "cpt-string.h"

namespace pathutil
{
    enum class PathStyle : char
    {
        kWindows,
        kUnix,
        kAuto
    };
    
    // 风格转换（unix与windows路径风格自适应），并移除重复的斜杠
    void PathStyleConvert(CptString& path, PathStyle to_style = PathStyle::kAuto);
    
    // 路径拼接
    CptString PathCombines(const CptString& dir, const CptString& filename);
    
    // 获取父目录
    CptString GetDirOfPathName(const CptString& path);
    
    CptString GetShortNameOfPath(const CptString& path);

    /* 
        相对路径转绝对路径，支持./与../开头的路径，
        支持连续的../，如../../../1.txt
        不支持.././.././../这种间隔的相对路径
    */
    CptString RelativeToAbsolute(const CptString& reltpath);
};













#endif // _UTIL_PATH_H_
