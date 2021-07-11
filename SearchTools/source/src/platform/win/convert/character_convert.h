

#pragma once

char* AnsiToUTF8(const char* str_ansi);
char* UTF8ToAnsi(const char* str_utf8);
wchar_t* UTF8ToWideString(const char* utf8);
char* WideStringToUTF8(const wchar_t* widechar);
wchar_t* AnsiToWideString(const char* ansi);
char* WideStringToAnsi(const wchar_t* widechar);
