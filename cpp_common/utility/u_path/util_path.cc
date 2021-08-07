

#include "util_path.h"

#include <assert.h>

#ifdef __unix__
#include <dirent.h>
#include <unistd.h>
#else   // windows
#include <Shlwapi.h>
#include <direct.h>

#pragma comment(lib, "shlwapi.lib")
#endif


#include <algorithm>

namespace pathutil
{
    static void _PathStyleConvert(std::string& path, bool to_forward)
    {
        char src_style, replace_style;
        size_t pos = 0, offset = 0;
        bool repeat = false;

        if (to_forward)
        {
            src_style = '\\', replace_style = '/';
        }
        else
        {
            src_style = '/', replace_style = '\\';
        }

        while (offset != std::string::npos)
        {
            pos = path.find(src_style, offset);
            if (pos == std::string::npos) {
                break;
            }
            path.replace(pos, 1, 1, replace_style);
            offset = pos + 1;
        }

        std::string target(2, replace_style);
        pos = 0, offset = 0;
        while (offset != std::string::npos)
        {
            pos = path.find(target, offset);
            if (pos != std::string::npos) {
                path.erase(pos + 1, 1);
            }
            offset = pos;
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
        if (dir.empty()) {
            return filename;
        }

        std::string ret = dir + "/" + filename;
        PathStyleConvert(ret);
        return ret;
    }
    
    // 获取父目录
    std::string GetDirOfPathName(const std::string& path)
    {
        std::string ret_str = path;
        PathStyleConvert(ret_str);

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

        PathStyleConvert(tmp, PathStyle::kUnix);

        size_t pos = tmp.rfind("/");
        if (pos == std::string::npos) {
            return tmp;
        }
        return tmp.substr(pos + 1);
    }

    std::string RelativeToAbsolute(const std::string& reltpath)
    {
        if (reltpath.size() < 2)
        {
            return "";
        }

        if (reltpath[0] != '.' ||
            (reltpath[1] != '\\' && reltpath[1] != '/'))
        {
            int exist;
#ifndef WIN32
            exist = access(reltpath.c_str(), F_OK) == 0;
#else
            exist = PathFileExistsA(reltpath.c_str());
#endif
            return exist ? reltpath : "";
        }

        char dir[512];
        size_t len = 512;
#ifdef WIN32
        if (!_getcwd(dir, 512)) {
            return "";
        }
#elif defined __unix__
        if (!getcwd(dir, 512)) {
            return "";
        }
#endif
        
        return PathCombines(dir, reltpath.substr(1));
    }

};

    
    
    