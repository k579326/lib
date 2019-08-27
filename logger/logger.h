

#ifndef _LOGGER_H_
#define _LOGGER_H_


#include "logger_define.h"

#define LOG_Debug(format, ...)      _log_print_(kDebugLevel, __FILE__, __FUNCTION__, __LINE__, format, __VA_ARGS__)
#define LOG_Infor(format, ...)      _log_print_(kInforLevel, __FILE__, __FUNCTION__, __LINE__, format, __VA_ARGS__)
#define LOG_Warning(format, ...)    _log_print_(kWarningLevel, __FILE__, __FUNCTION__, __LINE__, format, __VA_ARGS__)
#define LOG_Error(format, ...)      _log_print_(kErrorLevel, __FILE__, __FUNCTION__, __LINE__, format, __VA_ARGS__)
#define LOG_Fatal(format, ...)      _log_print_(kFatalLevel, __FILE__, __FUNCTION__, __LINE__, format, __VA_ARGS__)


typedef struct 
{
    LogLevels   level;
    RunModel    runModel;
    int         columns;
    LogOutput   outputModel;
}LogInitInfo;


int LogInit(const LogInitInfo& info, const char* version, const char* path);
            
void _log_print_(LogLevels level, 
                const char* filename, 
                const char* function, 
                int linenum, 
                const char* format, ...);






#endif


