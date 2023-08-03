
#include "stringdef.h"

const std::map<LogLevels, CptString> g_loglevel_str =
{
    { kDebugLevel, TEXT("DEBUG") },
    { kInforLevel, TEXT("INFO ") },
    { kWarningLevel, TEXT("WARN ") },
    { kErrorLevel, TEXT("ERROR") },
    { kFatalLevel, TEXT("FATAL") },
};







