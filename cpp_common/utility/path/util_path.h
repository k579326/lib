#ifndef _UTIL_PATH_H_
#define _UTIL_PATH_H_

#include <assert.h>

#include <string>
#include <algorithm>
#include "cpt-string.h"

#if defined __unix__ || defined __APPLE__
#include <dirent.h>
#include <unistd.h>
#endif

namespace
{
    template<class T>
    static void _PathStyleConvert(T& path, bool to_forward)
    {
        typename T::value_type src_style, replace_style;
        size_t pos = 0, offset = 0;
        bool repeat = false;

        if (to_forward)
        {
            src_style = (uint8_t)('\\'), replace_style = (uint8_t)('/');
        }
        else
        {
            src_style = (uint8_t)('/'), replace_style = (uint8_t)('\\');
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

        T target(2, replace_style);
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
}



namespace pathutil
{
    enum class PathStyle : char
    {
        kWindows,
        kUnix,
        kAuto
    };

    template<class String>
    struct Traits{};
    
    template<>
    struct Traits<std::string>
    {
        constexpr static const char* const slash = "/";
        constexpr static const char* const backslash = "\\";
        constexpr static const char* const localmark = "./";
        constexpr static const char* const prevmark = "../";
    };
    template<>
    struct Traits<std::wstring>
    {
        constexpr static const wchar_t* const slash = L"/";
        constexpr static const wchar_t* const backslash = L"\\";
        constexpr static const wchar_t* const localmark = L"./";
        constexpr static const wchar_t* const prevmark = L"../";
    };

    // 风格转换（unix与windows路径风格自适应），并移除重复的斜杠
    template<class T>
    void PathStyleConvert(T& path, PathStyle to_style = PathStyle::kAuto)
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
    template<class String, class Traits = Traits<String>>
    String PathsCombine(const String& dir, const String& filename)
    {
        if (dir.empty()) {
            return filename;
        }

        String finalpath = dir + Traits::slash + filename;
        PathStyleConvert<String>(finalpath);
        return finalpath;
    }

    // 获取父目录
    template<class String>
    String GetDirOfPathName(const String& path)
    {
        String ret_str = path;
        PathStyleConvert(ret_str);

        auto it = std::find_if(ret_str.rbegin(), ret_str.rend(), [](typename String::value_type c)->bool
        {
            return c == '\\' || c == '/'; }
        );
        if (it == ret_str.rend())
        {
            return String();
        }

        return String(ret_str.begin(), it.base());
    }

    template<class String, class Traits = Traits<String>>
    String GetShortNameOfPath(const String& path)
    {
        String tmp = path;
        PathStyleConvert(tmp, PathStyle::kUnix);

        size_t pos = tmp.rfind(Traits::slash);
        if (pos == std::string::npos) {
            return tmp;
        }
        return tmp.substr(pos + 1);
    }


    /*
        相对路径转绝对路径，支持./与../开头的路径，
        支持连续的../，如../../../1.txt
        不支持.././.././../这种间隔的相对路径
    */
    template<class String, class Traits = Traits<String>>
    String RelativeToAbsolute(const String& reltpath)
    {
        using CType = typename String::value_type;

        if (reltpath.empty())
        {
            return String();
        }

        if (reltpath.size() >= 2)
        {
            CType c1 = reltpath[0];
            CType c2 = reltpath[1];

#if defined _WIN32 || defined WIN32
            bool is_en_char = (c1 >= 'a' && c1 <= 'z') ||
                (c1 >= 'A' && c1 <= 'Z');
            bool is_win_absolute_path = (c2 == ':' && is_en_char);
            if (is_win_absolute_path)
                return reltpath;
#else
            bool is_unix_absolute_path = (c1 == '/');
            if (is_unix_absolute_path)
                return reltpath;
#endif
        }

        CType dir[2048];
        size_t len = 2048;
#if defined WIN32 || defined _WIN32
#if defined _UNICODE || defined UNICODE
        if (!_wgetcwd(dir, len)) {
#else
        if (!_getcwd(dir, len)) {
#endif
            return String();
        }
#else
        if (!getcwd(dir, len)) {
            return String();
        }
#endif
        int dirlen = 0;
        String dirstring = dir;
        PathStyleConvert(dirstring, pathutil::PathStyle::kUnix);

        if (dirstring.back() == '/')
            dirstring.erase(dirstring.end() - 1);

        String s = reltpath;
        PathStyleConvert(s, pathutil::PathStyle::kUnix);
        int count = 0;
        int pos = 0;
        int dir_pos = dirstring.size();

        while (dir_pos != std::string::npos)
        {
            if (s.compare(pos, 3, Traits::prevmark) == 0)
            {
                pos += 3;
                dir_pos--;
                dir_pos = dirstring.rfind('/', dir_pos);
                continue;
            }
            break;
        }

        if (dir_pos == std::string::npos)
            return String();

        if (pos != 0)
        {
            s = s.substr(pos);
            dirstring = dirstring.substr(0, dir_pos);
        }

        if (s.compare(0, 2, Traits::localmark) == 0)
            s = s.substr(2);

        return PathsCombine(dirstring, s);
    };

};











#endif // _UTIL_PATH_H_
