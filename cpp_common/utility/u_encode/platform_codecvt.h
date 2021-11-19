#pragma once

#include <stddef.h>

#ifdef WIN32
typedef wchar_t cv_char16;
#else
typedef unsigned short cv_char16;
#endif 

/*!
 * NOTES: inbytes is the byte count of input string, not contain the NULL teminated;
 *  and outbytes same to it. 
*/

char*       AnsiToUtf8(const char* s, size_t inbytes, size_t* outbytes);
cv_char16*    AnsiToUtf16LE(const char* s, size_t inbytes, size_t* outbytes);
char*       Utf8ToAnsi(const char* s, size_t inbytes, size_t* outbytes);
cv_char16*    Utf8ToUtf16LE(const char* s, size_t inbytes, size_t* outbytes);
char*       Utf16LEToAnsi(const cv_char16* s, size_t inbytes, size_t* outbytes);
char*       Utf16LEToUtf8(const cv_char16* s, size_t inbytes, size_t* outbytes);


#ifdef __cplusplus
#include <string>

std::string AnsiStringToU8String(const std::string& ansi);
std::wstring AnsiStringToU16String(const std::string& ansi);
std::string U16StringToAnsiString(const std::wstring& u16);
std::string U16StringToU8String(const std::wstring& u16);

std::string U8StringToAnsiString(const std::string& u8);
std::wstring U8StringToU16String(const std::string& u8);

#endif





