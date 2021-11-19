
#include "platform_codecvt.h"

#ifdef WIN32

#include <Windows.h>

char* AnsiToUtf8(const char* s, size_t inbytes, size_t* outbytes)
{
    int wlen = 0;
    wchar_t* wbuf = 0;
    char* utf8 = 0;

    // ANSI -> UNICODE
    wlen = MultiByteToWideChar(CP_ACP, 0, s, inbytes, NULL, 0);
    wbuf = (wchar_t*)malloc((wlen + 1) * sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, 0, s, inbytes, wbuf, wlen);
    wbuf[wlen] = 0;

    // UNICODE -> UTF8
    int ulen = WideCharToMultiByte(CP_UTF8, 0, wbuf, wlen, NULL, 0, NULL, NULL);
    utf8 = (char*)malloc((ulen + 1) * sizeof(char));
    WideCharToMultiByte(CP_UTF8, 0, wbuf, wlen, utf8, ulen, NULL, NULL);
    utf8[ulen] = 0;

    free(wbuf);
    *outbytes = ulen;
    return utf8;
}
wchar_t* AnsiToUtf16LE(const char* s, size_t inbytes, size_t* outbytes)
{
    int wlen = 0;
    wchar_t* wbuf = 0;
    char* utf8 = 0;

    // ANSI -> UNICODE
    wlen = MultiByteToWideChar(CP_ACP, 0, s, inbytes, NULL, 0);
    wbuf = (wchar_t*)malloc((wlen + 1) * sizeof(wchar_t));

    MultiByteToWideChar(CP_ACP, 0, s, inbytes, wbuf, wlen);
    wbuf[wlen] = 0;
    *outbytes = wlen * sizeof(wchar_t);
    return wbuf;
}
char* Utf8ToAnsi(const char* s, size_t inbytes, size_t* outbytes)
{
    int wlen = 0;
    wchar_t* wbuf = 0;
    char* ansi = 0;

    // str_utf8 -> UNICODE
    wlen = MultiByteToWideChar(CP_UTF8, 0, s, inbytes, NULL, 0);
    wbuf = (wchar_t*)malloc((wlen + 1) * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, s, inbytes, wbuf, wlen);
    wbuf[wlen] = 0;

    // UNICODE -> ansi
    int alen = WideCharToMultiByte(CP_ACP, 0, wbuf, wlen, NULL, 0, NULL, NULL);
    ansi = (char*)malloc((alen + 1) * sizeof(char));
    WideCharToMultiByte(CP_ACP, 0, wbuf, wlen, ansi, alen, NULL, NULL);
    ansi[alen] = 0;

    free(wbuf);
    *outbytes = alen;
    return ansi;
}
wchar_t* Utf8ToUtf16LE(const char* s, size_t inbytes, size_t* outbytes)
{
    int wlen = 0;
    wchar_t* wbuf = 0;

    // str_utf8 -> UNICODE
    wlen = MultiByteToWideChar(CP_UTF8, 0, s, inbytes, NULL, 0);
    wbuf = (wchar_t*)malloc((wlen + 1) * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, s, inbytes, wbuf, wlen);
    wbuf[wlen] = 0;
    *outbytes = wlen * sizeof(wchar_t);

    return wbuf;
}
char* Utf16LEToAnsi(const wchar_t* s, size_t inbytes, size_t* outbytes)
{
    char* ansi = NULL;
    int wlen = 0;
    // UNICODE -> ansi
    int inlen = inbytes / sizeof(wchar_t);
    int alen = WideCharToMultiByte(CP_ACP, 0, s, inlen, NULL, 0, NULL, NULL);
    ansi = (char*)malloc((alen + 1) * sizeof(char));
    WideCharToMultiByte(CP_ACP, 0, s, inlen, ansi, alen, NULL, NULL);
    ansi[alen] = 0;
    *outbytes = alen;
    return ansi;
}
char* Utf16LEToUtf8(const wchar_t* s, size_t inbytes, size_t* outbytes)
{
    int wlen = 0;
    char* utf8 = NULL;
    // UNICODE -> UTF8
    int inlen = inbytes / sizeof(wchar_t);
    int ulen = WideCharToMultiByte(CP_UTF8, 0, s, inlen, NULL, 0, NULL, NULL);
    utf8 = (char*)malloc((ulen + 1) * sizeof(char));
    WideCharToMultiByte(CP_UTF8, 0, s, inlen, utf8, ulen, NULL, NULL);
    utf8[ulen] = 0;
    *outbytes = ulen;

    return utf8;
}



std::string AnsiStringToU8String(const std::string& ansi)
{
    size_t outbytes = 0;
    char* u8 = AnsiToUtf8(ansi.c_str(), ansi.size(), &outbytes);
    std::string ret = u8 ? u8 : "";
    free(u8);
    return ret;
}
std::wstring AnsiStringToU16String(const std::string& ansi)
{
    size_t outbytes = 0;
    wchar_t* u16 = AnsiToUtf16LE(ansi.c_str(), ansi.size(), &outbytes);
    std::wstring ret = u16 ? u16 : L"";
    free(u16);
    return ret;
}
std::string U16StringToAnsiString(const std::wstring& u16)
{
    size_t outbytes = 0;
    char* a = Utf16LEToAnsi(u16.c_str(), u16.size() * sizeof(wchar_t), &outbytes);
    std::string ansi = a ? a : "";
    free(a);
    return ansi;
}

std::string U16StringToU8String(const std::wstring& u16)
{
    size_t outbytes = 0;
    char* u8 = Utf16LEToUtf8(u16.c_str(), u16.size() * sizeof(wchar_t), &outbytes);
    std::string ret = u8 ? u8 : "";
    free(u8);
    return ret;
}

std::string U8StringToAnsiString(const std::string& u8)
{
    size_t outbytes = 0;
    char* a = Utf8ToAnsi(u8.c_str(), u8.size(), &outbytes);
    std::string ret = a ? a : "";
    free(a);
    return ret;
}
std::wstring U8StringToU16String(const std::string& u8)
{
    size_t outbytes = 0;
    wchar_t* u16 = Utf8ToUtf16LE(u8.c_str(), u8.size(), &outbytes);
    std::wstring ret = u16 ? u16 : L"";
    free(u16);
    return ret;
}



#endif // WIN32

#if defined __unix__  || defined __unix

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iconv.h>


char* AnsiToUtf8(const char* s, size_t inbytes, size_t* outbytes)
{
    iconv_t cv = iconv_open("UTF8", "GB2312");
    if (cv == (iconv_t)-1) {
        *outbytes = 0;
        return nullptr;
    }
    
    char* in = const_cast<char*>(s);
    size_t outlen = inbytes * 4 + 1;
    char* buf = (char*)malloc(outlen);
    memset(buf, 0, outlen);
    
    size_t rcd = outlen;
    char* out = buf;
    size_t ret = iconv(cv, &in, &inbytes,
                    &out, &outlen);
                            
    if (ret == -1) {
        free(buf);
        *outbytes = 0;
        return NULL;
    }
    
    *outbytes = rcd - outlen;
    return buf;
}
cv_char16* AnsiToUtf16LE(const char* s, size_t inbytes, size_t* outbytes)
{
    iconv_t cv = iconv_open("UTF16LE", "GB2312");
    if (cv == (iconv_t)-1) {
        *outbytes = 0;
        return nullptr;
    }
    
    char* in = const_cast<char*>(s);
    size_t outlen = inbytes * 3 +  sizeof(cv_char16);
    cv_char16* buf = (cv_char16*)malloc(outlen);
    memset(buf, 0, outlen);
    
    char* out = (char*)buf;
    size_t rcd = outlen;
    size_t ret = iconv(cv, &in, &inbytes,
                    &out, &outlen);
                            
    if (ret == -1) {
        free(buf);
        *outbytes = 0;
        return NULL;
    }
    
    *outbytes = rcd - outlen;
    return buf;
}
char* Utf8ToAnsi(const char* s, size_t inbytes, size_t* outbytes)
{
    iconv_t cv = iconv_open("GB2312", "UTF8");
    if (cv == (iconv_t)-1) {
        *outbytes = 0;
        return nullptr;
    }
    
    char* in = const_cast<char*>(s);
    size_t outlen = inbytes * 2 + sizeof(char);
    char* buf = (char*)malloc(outlen);
    memset(buf, 0, outlen);
    
    char* out = buf;
    size_t rcd = outlen;
    size_t ret = iconv(cv, &in, &inbytes,
                    (char**)&out, &outlen);
                            
    if (ret == -1) {
        free(buf);
        *outbytes = 0;
        return NULL;
    }
    
    *outbytes = rcd - outlen;
    return buf;
}
cv_char16* Utf8ToUtf16LE(const char* s, size_t inbytes, size_t* outbytes)
{
    iconv_t cv = iconv_open("UTF16LE", "UTF8");
    if (cv == (iconv_t)-1) {
        *outbytes = 0;
        return nullptr;
    }
    
    char* in = const_cast<char*>(s);
    size_t outlen = inbytes * 2 + sizeof(cv_char16);
    cv_char16* buf = (cv_char16*)malloc(outlen);
    memset(buf, 0, outlen);
    
    char* out = (char*)buf;
    size_t rcd = outlen;
    size_t ret = iconv(cv, &in, &inbytes,
                    (char**)&out, &outlen);
                            
    if (ret == -1) {
        free(buf);
        *outbytes = 0;
        return NULL;
    }
    
    *outbytes = rcd - outlen;
    return buf;
}
char* Utf16LEToAnsi(const cv_char16* s, size_t inbytes, size_t* outbytes)
{
    iconv_t cv = iconv_open("GB2312", "UTF16LE");
    if (cv == (iconv_t)-1) {
        *outbytes = 0;
        return nullptr;
    }
    
    cv_char16* in = const_cast<cv_char16*>(s);
    size_t outlen = inbytes * 2 + 1;
    char* buf = (char*)malloc(outlen);
    memset(buf, 0, outlen);
    
    char* out = buf;
    size_t rcd = outlen;
    size_t ret = iconv(cv, (char**)&in, &inbytes,
                    (char**)&out, &outlen);
                            
    if (ret == -1) {
        free(buf);
        *outbytes = 0;
        return NULL;
    }
    
    *outbytes = rcd - outlen;
    return buf;
    
}
char* Utf16LEToUtf8(const cv_char16* s, size_t inbytes, size_t* outbytes)
{
    iconv_t cv = iconv_open("UTF8", "UTF16LE");
    if (cv == (iconv_t)-1) {
        *outbytes = 0;
        return nullptr;
    }
    
    cv_char16* in = const_cast<cv_char16*>(s);
    size_t outlen = inbytes * 2 + 1;
    char* buf = (char*)malloc(outlen);
    memset(buf, 0, outlen);
    
    char* out = buf;
    size_t rcd = outlen;
    size_t ret = iconv(cv, (char**)&in, &inbytes,
                    (char**)&out, &outlen);
                            
    if (ret == -1) {
        free(buf);
        *outbytes = 0;
        return NULL;
    }
    
    *outbytes = rcd - outlen;
    return buf;
}




#endif // linux











