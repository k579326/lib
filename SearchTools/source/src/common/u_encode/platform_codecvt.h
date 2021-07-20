#pragma once

#ifdef __cplusplus
extern "C" {
#endif

    char*       AnsiToUtf8(const char* s, size_t intcount, size_t* outcount);
    wchar_t*    AnsiToUtf16LE(const char* s, size_t intcount, size_t* outcount);
    char*       Utf8ToAnsi(const char* s, size_t intcount, size_t* outcount);
    wchar_t*    Utf8ToUtf16LE(const char* s, size_t intcount, size_t* outcount);
    char*       Utf16LEToAnsi(const wchar_t* s, size_t intcount, size_t* outcount);
    char*       Utf16LEToUtf8(const wchar_t* s, size_t intcount, size_t* outcount);

#ifdef __cplusplus
}
#endif


