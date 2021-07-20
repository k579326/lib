
#include "platform_codecvt.h"

#include <Windows.h>

char* AnsiToUtf8(const char* s, size_t intcount, size_t* outcount)
{
    int wlen = 0;
    wchar_t* wbuf = 0;
    char* utf8 = 0;

    // ANSI -> UNICODE
    wlen = MultiByteToWideChar(CP_ACP, 0, s, intcount, NULL, 0);
    wbuf = (wchar_t*)malloc((wlen + 1) * sizeof(wchar_t));
    if (NULL == wbuf)
    {
        return NULL;
    }
    MultiByteToWideChar(CP_ACP, 0, s, intcount, wbuf, wlen);
    wbuf[wlen] = 0;

    // UNICODE -> UTF8
    int ulen = WideCharToMultiByte(CP_UTF8, 0, wbuf, wlen, NULL, 0, NULL, NULL);
    utf8 = (char*)malloc((ulen + 1) * sizeof(char));
    if (NULL == utf8)
    {
        return NULL;
    }
    WideCharToMultiByte(CP_UTF8, 0, wbuf, wlen, utf8, ulen, NULL, NULL);
    utf8[ulen] = 0;

    free(wbuf);
    *outcount = ulen;
    return utf8;
}
wchar_t* AnsiToUtf16LE(const char* s, size_t intcount, size_t* outcount)
{
    int wlen = 0;
    wchar_t* wbuf = 0;
    char* utf8 = 0;

    // ANSI -> UNICODE
    wlen = MultiByteToWideChar(CP_ACP, 0, s, intcount, NULL, 0);
    wbuf = (wchar_t*)malloc((wlen + 1) * sizeof(wchar_t));

    MultiByteToWideChar(CP_ACP, 0, s, intcount, wbuf, wlen);
    wbuf[wlen] = 0;
    *outcount = wlen;
    return wbuf;
}
char* Utf8ToAnsi(const char* s, size_t intcount, size_t* outcount)
{
    int wlen = 0;
    wchar_t* wbuf = 0;
    char* ansi = 0;

    // str_utf8 -> UNICODE
    wlen = MultiByteToWideChar(CP_UTF8, 0, s, intcount, NULL, 0);
    wbuf = (wchar_t*)malloc((wlen + 1) * sizeof(wchar_t));
    if (NULL == wbuf)
    {
        return NULL;
    }
    MultiByteToWideChar(CP_UTF8, 0, s, intcount, wbuf, wlen);
    wbuf[wlen] = 0;

    // UNICODE -> ansi
    int alen = WideCharToMultiByte(CP_ACP, 0, wbuf, wlen, NULL, 0, NULL, NULL);
    ansi = (char*)malloc((alen + 1) * sizeof(char));
    if (NULL == ansi)
    {
        return NULL;
    }
    WideCharToMultiByte(CP_ACP, 0, wbuf, wlen, ansi, alen, NULL, NULL);
    ansi[alen] = 0;

    free(wbuf);
    *outcount = alen;
    return ansi;
}
wchar_t* Utf8ToUtf16LE(const char* s, size_t intcount, size_t* outcount)
{
    int wlen = 0;
    wchar_t* wbuf = 0;

    // str_utf8 -> UNICODE
    wlen = MultiByteToWideChar(CP_UTF8, 0, s, intcount, NULL, 0);
    wbuf = (wchar_t*)malloc((wlen + 1) * sizeof(wchar_t));
    if (NULL == wbuf)
    {
        return NULL;
    }
    MultiByteToWideChar(CP_UTF8, 0, s, intcount, wbuf, wlen);
    wbuf[wlen] = 0;
    *outcount = wlen;

    return wbuf;
}
char* Utf16LEToAnsi(const wchar_t* s, size_t intcount, size_t* outcount)
{
    char* ansi = NULL;
    int wlen = 0;
    // UNICODE -> ansi
    int alen = WideCharToMultiByte(CP_ACP, 0, s, intcount, NULL, 0, NULL, NULL);
    ansi = (char*)malloc((alen + 1) * sizeof(char));
    if (NULL == ansi)
    {
        return NULL;
    }
    WideCharToMultiByte(CP_ACP, 0, s, intcount, ansi, alen, NULL, NULL);
    ansi[alen] = 0;
    *outcount = alen;
    return ansi;
}
char* Utf16LEToUtf8(const wchar_t* s, size_t intcount, size_t* outcount)
{
    int wlen = 0;
    char* utf8 = NULL;
    // UNICODE -> UTF8
    int ulen = WideCharToMultiByte(CP_UTF8, 0, s, intcount, NULL, 0, NULL, NULL);
    utf8 = (char*)malloc((ulen + 1) * sizeof(char));
    if (NULL == utf8)
    {
        return NULL;
    }
    WideCharToMultiByte(CP_UTF8, 0, s, intcount, utf8, ulen, NULL, NULL);
    utf8[ulen] = 0;
    *outcount = ulen;

    return utf8;
}