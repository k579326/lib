
#include "formatter.h"

#include <ctime>
#include <iostream>
#include <chrono>
#include <sstream>
#include <string>
#include <thread>
#include <map>

#include "path/util_path.h"
#include "stringdef/stringdef.h"
#include "thread_id.h"

/* 当前输出的日志的等级低于LogLevels时，对应的字段LogColumns将被输出，否则将被忽略 */
const static std::map<LogColumns, LogLevels> g_column_config = 
{
    {kDateTime, kLevelEnd},
    {kThreadId, kLevelEnd},
    {kLogLevel, kLevelEnd},
    {kFileInfo, kErrorLevel},
    {kFunction, kFatalLevel},
    {kVersion,  kLevelEnd}
};


Formatter::Formatter(int column, const CptString& version) : column_(column), version_(version)
{
}

Formatter::~Formatter() { }

CptString Formatter::Format(LogLevels level, const CptString& filename, const CptString& function, int linenum) const
{
    CptString buf;

    int mask = AdjustOutputColumn(level);

    if (mask & kDateTime)
    {
        CptChar timestr[256];
        std::chrono::milliseconds chrono_ms = 
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        time_t timestamp = chrono_ms.count() / 1000;
        const uint16_t ms = chrono_ms.count() % 1000;
        tm* tp = localtime(&timestamp);
        CptStringPrintf(timestr, TEXT("%4d-%02d-%02d %02d:%02d:%02d.%03d"), tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday,
            tp->tm_hour, tp->tm_min, tp->tm_sec, ms);

        buf.append(timestr);
        buf.append(TEXT(" "));
    }
    if (mask & kLogLevel)
    {
        buf.append(g_loglevel_str.find(level)->second);
        buf.append(TEXT(" "));
    }
    if (mask & kVersion)
    {
        CptString ver;
        ver = TEXT("[") + version_ + TEXT("]");
        buf.append(ver);
        buf.append(TEXT(" "));
    }
    if (mask & kThreadId)
    {
        CptString idstr;
        uint32_t tid = GetSelfThreadId();

        idstr = TEXT("#") + ToCptString(tid);
        buf.append(idstr);
        buf.append(TEXT(" "));
    }
    if (mask & kFileInfo)
    {
        CptString fileinfo;
        CptString shortname = pathutil::GetShortNameOfPath(filename);

        fileinfo = shortname + TEXT("(") + ToCptString(linenum) + TEXT(")");
        buf.append(fileinfo);
        buf.append(TEXT(" "));
    }
    if (mask & kFunction)
    {
        buf.append(function);
        buf.append(TEXT(" "));
    }
    return buf;
}

int Formatter::AdjustOutputColumn(LogLevels level) const
{
    int column_mask = column_;
    for (auto& e : g_column_config)
    {
        if (level >= e.second)
        {
            column_mask &= ~e.first;
        }
    }

    return column_mask;
}