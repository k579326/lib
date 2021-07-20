

#include "util_path.h"

#include <assert.h>

#include <algorithm>

namespace pathutil
{
    static void _PathStyleConvert(std::string& path, bool to_forward)
    {
        char src_style, replace_style;
        size_t pos = 0, off = 0;
        
        if (to_forward)
        {
            src_style = '\\', replace_style = '/';
        }
        else
        {
            src_style = '/', replace_style = '\\';
        }

        while (off != std::string::npos)
        {
            pos = path.find(src_style, off);
            if (pos == std::string::npos) {
                break;
            }
            path.replace(pos, 1, 1, replace_style);
            off = pos + 1;
        }

        return;
    }
    
    // 风格转换（unix与windows路径风格自适应）
    void PathStyleConvert(std::string& path, PathStyle to_style)
    {
        if (to_style == PathStyle::kAuto) {
            #ifdef WIN32
            _PathStyleConvert(path, false);
            #else
            _PathStyleConvert(path, true);
            #endif
        }
        else if (to_style == PathStyle::kWindows) {
            _PathStyleConvert(path, false);
        }
        else if (to_style == PathStyle::kUnix)
        {
            _PathStyleConvert(path, true);
        }
        else {
            assert(0);
        }
        return;
    }
    
    // 路径拼接
    std::string PathCombines(const std::string& dir, const std::string& filename)
    {
        if (dir[dir.size() - 1] == '/' || dir[dir.size() - 1] == '\\')
            return dir + filename;
        
        return dir + "/" + filename;
    }
    
    // 获取父目录
    std::string GetDirOfPathName(const std::string& path)
    {
        std::string ret_str = path;

        auto it = std::find_if(ret_str.rbegin(), ret_str.rend(), [](char c)->bool { return c == '\\' || c == '/'; });
        if (it == ret_str.rend())
        {
            return "";
        }

        return std::string(ret_str.begin(), it.base());
    }
  
    std::string GetShortNameOfPath(const std::string& path)
    {
        std::string tmp = path;

        PathStyleConvert(tmp, kUnix);

        size_t pos = tmp.rfind("/");
        if (pos == std::string::npos) {
            return tmp;
        }
        return tmp.substr(pos + 1);
    }


};

    
    
    