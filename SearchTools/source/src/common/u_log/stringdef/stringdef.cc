
#include "stringdef.h"


const std::map<LogLevels, std::string> g_loglevel_str =
{
    {kDebugLevel, "DEBUG"},
    {kInforLevel, "INFO "},
    {kWarningLevel, "WARN "},
    {kErrorLevel, "ERROR"},
    {kFatalLevel, "FATAL"},
};


const std::map<RunModel, std::string> g_runmodel_str = 
{
    {kSync, "SYNC"},
    {kAsync, "ASYNC"}    
};






