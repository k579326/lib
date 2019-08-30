
#include "util_chset_cvt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <codecvt>

#if 0
char* AnsiToUTF8(const char* str_ansi)
{
     int wlen = 0;
     wchar_t* wbuf = 0;
     char* utf8 = 0;
 
     // ANSI -> UNICODE
     wlen = MultiByteToWideChar(CP_ACP, 0, str_ansi, -1, NULL, 0);
     wbuf = (wchar_t*)malloc((wlen + 1)*sizeof(wchar_t));
     if(NULL == wbuf)
     {
        return NULL;
     }
     MultiByteToWideChar(CP_ACP, 0, str_ansi, -1, wbuf, wlen);
     wbuf[wlen] = 0;
 
     // UNICODE -> UTF8
     int ulen = WideCharToMultiByte(CP_UTF8, 0, wbuf, wlen, NULL, 0, NULL, NULL);
     utf8 = (char*)malloc((ulen + 1)*sizeof(char));
     if(NULL == utf8)
     {
        return NULL;
     }
     WideCharToMultiByte(CP_UTF8, 0, wbuf, wlen, utf8, ulen, NULL, NULL);
     utf8[ulen] = 0;
	 
     free(wbuf);
	 
     return utf8;
}

char* UTF8ToAnsi(const char* str_utf8)
{
     int wlen = 0;
     wchar_t* wbuf = 0;
     char* ansi = 0;
 
     // str_utf8 -> UNICODE
     wlen = MultiByteToWideChar(CP_UTF8, 0, str_utf8, -1, NULL, 0);
     wbuf = (wchar_t*)malloc((wlen + 1) * sizeof(wchar_t));
     if(NULL == wbuf)
     {
        return NULL;
     }
     MultiByteToWideChar(CP_UTF8, 0, str_utf8, -1, wbuf, wlen);
     wbuf[wlen] = 0;
 
     // UNICODE -> ansi
     int alen = WideCharToMultiByte(CP_ACP, 0, wbuf, wlen, NULL, 0, NULL, NULL);
     ansi = (char*)malloc((alen + 1) * sizeof(char));
     if(NULL == ansi)
     {
        return NULL;
     }
     WideCharToMultiByte(CP_ACP, 0, wbuf, wlen, ansi, alen, NULL, NULL);
     ansi[alen] = 0;
 
     free(wbuf);
     return ansi;
}

wchar_t* UTF8ToWideString(const char* utf8)
{
    int wlen = 0;
    wchar_t* wbuf = 0;

    // str_utf8 -> UNICODE
    wlen = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    wbuf = (wchar_t*)malloc((wlen + 1) * sizeof(wchar_t));
    if (NULL == wbuf)
    {
        return NULL;
    }
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wbuf, wlen);
    wbuf[wlen] = 0;

    return wbuf;
}

char* WideStringToUTF8(const wchar_t* widechar)
{
    int wlen = 0;
    char* utf8 = NULL;
    // UNICODE -> UTF8
    int ulen = WideCharToMultiByte(CP_UTF8, 0, widechar, -1, NULL, 0, NULL, NULL);
    utf8 = (char*)malloc((ulen + 1)*sizeof(char));
    if (NULL == utf8)
    {
        return NULL;
    }
    WideCharToMultiByte(CP_UTF8, 0, widechar, -1, utf8, ulen, NULL, NULL);
    utf8[ulen] = 0;

    return utf8;
}

wchar_t* AnsiToWideString(const char* ansi)
{
    int wlen = 0;
    wchar_t* wbuf = 0;
    char* utf8 = 0;

    // ANSI -> UNICODE
    wlen = MultiByteToWideChar(CP_ACP, 0, ansi, -1, NULL, 0);
    wbuf = (wchar_t*)malloc((wlen + 1)*sizeof(wchar_t));
   
    MultiByteToWideChar(CP_ACP, 0, ansi, -1, wbuf, wlen);
    wbuf[wlen] = 0;

    return wbuf;
}

char* WideStringToAnsi(wchar_t* widechar)
{
    char* ansi = NULL;
    int wlen = 0;
    // UNICODE -> ansi
    int alen = WideCharToMultiByte(CP_ACP, 0, widechar, -1, NULL, 0, NULL, NULL);
    ansi = (char*)malloc((alen + 1) * sizeof(char));
    if (NULL == ansi)
    {
        return NULL;
    }
    WideCharToMultiByte(CP_ACP, 0, widechar, -1, ansi, alen, NULL, NULL);
    ansi[alen] = 0;

    return ansi;
}
#endif

namespace chsetutil
{
    // string的编码方式为utf8，则采用：
    std::string WideStringToUTF8(const std::wstring& str)
    {
        static std::wstring_convert<std::codecvt_utf8<wchar_t> > strCnv;
        return strCnv.to_bytes(str);
    }

    std::wstring UTF8ToWideString(const std::string& str)
    {
        static std::wstring_convert< std::codecvt_utf8<wchar_t> > strCnv;
        return strCnv.from_bytes(str);
    }

    // string的编码方式为除utf8外的其它编码方式，可采用：
    std::string WideStringToLocal(const std::wstring& str, const std::string& locale)//locale = "chs"或"zh-cn"
    {
        typedef std::codecvt_byname<wchar_t, char, std::mbstate_t> F;
        static std::wstring_convert<F> strCnv(new F(locale));

        return strCnv.to_bytes(str);
    }

    std::wstring LocalToWideString(const std::string& str, const std::string& locale)
    {
        typedef std::codecvt_byname<wchar_t, char, std::mbstate_t> F;
        static std::wstring_convert<F> strCnv(new F(locale));

        return strCnv.from_bytes(str);
    }
    
};














