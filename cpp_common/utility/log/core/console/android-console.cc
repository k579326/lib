


#include "console_extern.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <android/log.h>

ConsoleID GetConsole() { return 0; }

bool SetConsoleTextColor(ConsoleID console, __LogTextColor color, bool intensity) { return true; }

int WriteConsole_(ConsoleID console, LogLevels level, const void* ptr, int len)
{
    int android_log_level = 0;
    switch (level)
    {
        case LogLevels::kDebugLevel:
            android_log_level = ANDROID_LOG_DEBUG;
            break;
        case LogLevels::kInforLevel:
            android_log_level = ANDROID_LOG_INFO;
            break;
        case LogLevels::kWarningLevel:
            android_log_level = ANDROID_LOG_WARN;
            break;
        case LogLevels::kErrorLevel:
            android_log_level = ANDROID_LOG_ERROR;
            break;
        case LogLevels::kFatalLevel:
            android_log_level = ANDROID_LOG_FATAL;
            break;
        case LogLevels::kLevelEnd:
            android_log_level = ANDROID_LOG_INFO;
            break;
        default:
            return -1;
    }

    __android_log_print(android_log_level, "LOGGER", "%s", ptr);
    return 0;
}

void RestoreConsole(ConsoleID console) {}


