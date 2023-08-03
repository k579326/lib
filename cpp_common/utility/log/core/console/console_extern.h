
#ifndef _CONSOLE_EXTERN_H_
#define _CONSOLE_EXTERN_H_

#include "logger_define.h"

#ifdef _WIN32
    #include <windows.h>
    typedef HANDLE ConsoleID;
#else
    #include <unistd.h>
    typedef int ConsoleID;
#endif


enum __LogTextColor
{
    Black = 0x00000000,             // 黑
    Red = 0x00FF0000,               // 红
    Green = 0x0000FF00,             // 绿
    Blue = 0x000000FF,              // 蓝
    Yellow = 0x00FFFF00,            // 黄
    Purple = 0x00FF00FF,            // 紫
    White = 0x00FFFFFF,             // 白
    Cyan = 0x0000FFFF,              // 青
    InvalidColor = -1
};


#ifdef __cplusplus
extern "C" {
#endif

ConsoleID GetConsole();

bool SetConsoleTextColor(ConsoleID console, __LogTextColor color, bool intensity);

int WriteConsole_(ConsoleID console, LogLevels level, const void* ptr, int len);

void RestoreConsole(ConsoleID console);


#ifdef __cplusplus
}
#endif



#endif // _CONSOLE_EXTERN_H_

