

#include "formatter.h"

#include <ctime>
#include <iostream>
#include <chrono>
#include <sstream>
#include <string>
#include <thread>
#include <map>

/* ��ǰ�������־�ĵȼ�����LogLevelsʱ����Ӧ���ֶ�LogColumns������������򽫱����� */
const static std::map<LogColumns, LogLevels> g_column_config = 
{
    {kDateTime, kLevelEnd},
    {kThreadId, kLevelEnd},
    {kLogLevel, kLevelEnd},
    {kFileInfo, kErrorLevel},
    {kFunction, kFatalLevel},
    {kVersion,  kLevelEnd}
};

const static std::map<LogLevels, std::string> g_loglevel_str =
{
    {kDebugLevel, "DEBUG"},
    {kInforLevel, "INFO "},
    {kWarningLevel, "WARN "},
    {kErrorLevel, "ERROR"},
    {kFatalLevel, "FATAL"},
};



Formatter::Formatter(int column, std::string version) : column_(column), version_(version)
{
}

Formatter::~Formatter() { }

std::string Formatter::Format(LogLevels level, const std::string& filename, const std::string& function, int linenum)
{
    std::string buf;

    int mask = AdjustOutputColumn(level);

    if (mask & kDateTime)
    {
        char timestr[256];
        time_t timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        tm* tp = localtime(&timestamp);
        sprintf(timestr, "%4d-%02d-%02d %02d:%02d:%02d", tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday,
            tp->tm_hour, tp->tm_min, tp->tm_sec);

        buf.append(timestr);
        buf.append(" ");
    }
    if (mask & kLogLevel)
    {
        buf.append(g_loglevel_str.find(level)->second);
        buf.append(" ");
    }
    if (mask & kVersion)
    {
        std::string ver;
        ver = "[" + version_ + "]";
        buf.append(ver);
        buf.append(" ");
    }
    if (mask & kThreadId)
    {
        std::string idstr;
        std::stringstream x;
        std::thread::id tid = std::this_thread::get_id();
        x << tid;
        x >> idstr;

        idstr = "#" + idstr;
        buf.append(idstr);
        buf.append(" ");
    }
    if (mask & kFileInfo)
    {
        std::string fileinfo;

        fileinfo = filename + "(" + std::to_string(linenum) + ")";
        buf.append(fileinfo);
        buf.append(" ");
    }
    if (mask & kFunction)
    {
        buf.append(function);
        buf.append(" ");
    }
    return buf;
}

int Formatter::AdjustOutputColumn(LogLevels level)
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