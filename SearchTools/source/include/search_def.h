#pragma once



#include <string>

#if !defined(_UNICODE) && !defined(UNICODE)
#define CPT(x) (x)
using CPTString = std::string;
define cptprintf printf
#else
#define CPT(x) (L##x)
using CPTString = std::wstring;
#define cptprintf wprintf
#endif 


