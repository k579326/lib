#pragma once


#include <string.h>

#ifdef __cplusplus

#include <string>
#ifdef CptString
#undef CptString
#endif

#if defined _UNICODE || defined UNICODE
	#define CptString			std::wstring
	#define ToCptString			std::to_wstring
#else
	#define CptString			std::string
	#define ToCptString			std::to_string
#endif
// string or wstring to Int, use std::stoi

#endif

#ifdef CptChar
#undef CptChar
#endif

#if defined _UNICODE || defined UNICODE
#define CptChar wchar_t
#define CptStringLength wcslen
#define CptStringCat	wcscat
#define CptStringCopy	wcscpy
#define CptStringCmp	wcscmp
#define CptPrintf		wprintf
#define CptStringPrintf swprintf
#define Cptvsprintf		vswprintf
#define Cptvsnprintf	vswprintf
#else
#define CptChar char
#define CptStringLength strlen
#define CptStringCat	strcat
#define CptStringCopy	strcpy
#define CptStringCmp	strcmp
#define CptPrintf		printf
#define CptStringPrintf sprintf
#define Cptvsprintf		vsprintf
#define Cptvsnprintf	vsnprintf
#endif

#if defined _UNICODE || defined UNICODE
#define __texT_(x) L##x
#ifndef TEXT
#define TEXT(x)	__texT_(x) 
#endif
#else
#define __texT_(x) x
#ifndef TEXT
#define TEXT(x)	x
#endif
#endif


#if defined _UNICODE || defined UNICODE
#define MACROSTRINGEXPAND(macro) __texT_(macro)
#else
#define MACROSTRINGEXPAND(macro) macro
#endif


