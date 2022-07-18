

#include "util_path.h"

#include <assert.h>

#if defined __unix__ || defined __APPLE__
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
    static void _PathStyleConvert(CptString& path, bool to_forward)
    {
        char src_style, replace_style;
        size_t pos = 0, offset = 0;
        bool repeat = false;

        if (to_forward)
        {
            src_style = TEXT('\\'), replace_style = TEXT('/');
        }
        else
        {
            src_style = TEXT('/'), replace_style = TEXT('\\');
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

        CptString target(2, replace_style);
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
    void PathStyleConvert(CptString& path, PathStyle to_style)
    {
        if (to_style == PathStyle::kAuto) {
#if defined WIN32 || defined _WIN32
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
    CptString PathCombines(const CptString& dir, const CptString& filename)
    {
        if (dir.empty()) {
            return filename;
        }

		CptString ret = dir + TEXT("/") + filename;
        PathStyleConvert(ret);
        return ret;
    }
    
    // 获取父目录
    CptString GetDirOfPathName(const CptString& path)
    {
        CptString ret_str = path;
        PathStyleConvert(ret_str);

        auto it = std::find_if(ret_str.rbegin(), ret_str.rend(), [](CptChar c)->bool { return c == TEXT('\\') || c == TEXT('/'); });
        if (it == ret_str.rend())
        {
            return TEXT("");
        }

        return CptString(ret_str.begin(), it.base());
    }
  
    CptString GetShortNameOfPath(const CptString& path)
    {
        CptString tmp = path;

        PathStyleConvert(tmp, PathStyle::kUnix);

        size_t pos = tmp.rfind(TEXT("/"));
        if (pos == std::string::npos) {
            return tmp;
        }
        return tmp.substr(pos + 1);
    }

    CptString RelativeToAbsolute(const CptString& reltpath)
    {
        if (reltpath.empty())
        {
            return TEXT("");
        }

        if (reltpath.size() >= 2)
        {
            CptChar c1 = reltpath[0];
            CptChar c2 = reltpath[1];

#if defined _WIN32 || defined WIN32
            bool is_en_char = (c1 >= TEXT('a') && c1 <= TEXT('z')) ||
                (c1 >= TEXT('A') && c1 <= TEXT('Z'));
            bool is_win_absolute_path = (c2 == TEXT(':') && is_en_char);
            if (is_win_absolute_path)
                return reltpath;
#else
            bool is_unix_absolute_path = (c1 == TEXT('/'));
            if (is_unix_absolute_path)
                return reltpath;
#endif
        }

        CptChar dir[2048];
        size_t len = 2048;
#if defined WIN32 || defined _WIN32
	#if defined _UNICODE || defined UNICODE
        if (!_wgetcwd(dir, len)) {
	#else
		if (!_getcwd(dir, len)) {
	#endif
            return TEXT("");
        }
#else
        if (!getcwd(dir, len)) {
            return TEXT("");
        }
#endif
        int dirlen = 0;
        CptString dirstring = dir;
        PathStyleConvert(dirstring, pathutil::PathStyle::kUnix);

        if (dirstring.back() == TEXT('/'))
            dirstring.erase(dirstring.end() - 1);

        CptString s = reltpath;
        PathStyleConvert(s, pathutil::PathStyle::kUnix);
        int count = 0;
        int pos = 0;
        int dir_pos = dirstring.size();

        while (dir_pos != std::string::npos)
        {
            if (s.compare(pos, 3, TEXT("../")) == 0)
            {
                pos += 3;
                dir_pos--;
                dir_pos = dirstring.rfind(TEXT('/'), dir_pos);
                continue;
            }
            break;
        }

        if (dir_pos == std::string::npos)
            return TEXT("");

        if (pos != 0)
        {
            s = s.substr(pos);
            dirstring = dirstring.substr(0, dir_pos);
        }

        if (s.compare(0, 2, TEXT("./")) == 0)
            s = s.substr(2);

        return PathCombines(dirstring, s);
    }

};

    
    
    