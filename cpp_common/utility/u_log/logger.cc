
#include "logger.h"

#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "u_path/util_path.h"
#include "u_dir/util_dir.h"

#include "core/formatter.h"
#include "core/printer.h"
#include "autoclear/logclear.h"
#include "stringdef/stringdef.h"

static const char* g_log_start = "\n\
**********************************************************\n\
**** Logger Startup Header                             \n\
**** Time:      %4d-%02d-%02d %02d:%02d:%02d           \n\
**** LogLevel:  %s                                     \n\
**** RunModel:  %s                                     \n\
**********************************************************\n\
\n";


bool Logger::isInited_ = false;

Logger* Logger::GetInstance()
{
    static Logger logger;
    return &logger;
}

bool Logger::IsInited()
{
    return isInited_;
}



Logger::Logger()
{
    memset(&loginfo_, 0, sizeof(loginfo_));
}
Logger::~Logger()
{

}


void Logger::Init(const LogInitInfo& info, const std::string& version, const std::string& path)
{
    if (isInited_)
    {
        return;
    }

    // init printer
    if (info.runModel == RunModel::kAsync) { 
        printer_.reset(new ThreadPrinter(info.outputModel));
    }
    else
    {
        printer_.reset(new SyncPrinter(info.outputModel));
    }

    std::string logpath = pathutil::PathCombines(path, "logs");
    CommCreateDir(logpath.c_str());

    fmt_ = std::make_shared<Formatter>(info.columns, version);
    lnm_.SetLabel(info.label);
    lnm_.SetPath(logpath);

    loginfo_ = info;
    version_ = version;
    logpath_ = logpath;

    clr_ = std::make_shared<LogClr>(logpath_, info.keep_days, &lnm_);
    clr_->Start(5); // 5秒检查一次

    printer_->SetIO(lnm_.GetLogName());
    SetLoggerHeader();

    isInited_ = true;
}

void Logger::SetLoggerHeader()
{
    char buf[2048];

    time_t timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    tm* tp = localtime(&timestamp);
    sprintf(buf, g_log_start, tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday,
        tp->tm_hour, tp->tm_min, tp->tm_sec, 
        g_loglevel_str.find(loginfo_.level)->second.c_str(), 
        g_runmodel_str.find(loginfo_.runModel)->second.c_str()
    );

    printer_->Output(buf, kInforLevel);
}


void Logger::Uninit()
{
    memset(&loginfo_, 0, sizeof(loginfo_));
    logpath_ = "";
    version_ = "";

    isInited_ = false;
}

void Logger::Print(LogLevels level, const std::string& filename, 
    const std::string& function, int linenum, const char* format, ...)
{
    if (level < loginfo_.level) { 
        return ;
    }

    std::string column_str = fmt_->Format(level, filename, function, linenum);

    char buf[640] = { 0 };
    va_list varArgs;
    va_start(varArgs, format);
    std::vsnprintf(buf, 640, format, varArgs);
    va_end(varArgs);

    if (lnm_.NeedUpdate()) {
        printer_->SetIO(lnm_.GetLogName());
    }

    printer_->Output(column_str + "###  " + buf + "\n", level);
}

char Logger::ConvertCharacter(const char& c)
{
    if ((uint8_t)c > 0x7eu || (uint8_t)c < 0x20u) {
        if ((uint8_t)c > 0x7eu)
            return '?';
        else
            return '.';
    }

    return c;
}

std::string Logger::BuildMemoryVisualData(const void* memory, size_t len)
{
    std::string ret;
    // mast large than (strlen(df) + strlen(af) + strlen(sf))
    ret.reserve(5 * len);

    if (MEMORY_WIDTH_EACHLEN < 8) {
        // bad setting
        return "";
    }

    size_t cursor = 0;
    char* m = reinterpret_cast<char*>(const_cast<void*>(memory));
    char df[MEMORY_WIDTH_EACHLEN * 3 + 1] = { 0 };
    char af[32] = {}, sf[MEMORY_WIDTH_EACHLEN + 1] = {};
    while (cursor < len)
    {
        if (cursor % MEMORY_WIDTH_EACHLEN == 0) {
            sprintf(af, "%p", m + cursor);
        }

        char byte[4];
        sprintf(byte, "%02x ", (uint8_t)m[cursor]);
        strcat(df, byte);

        sf[cursor % MEMORY_WIDTH_EACHLEN] = ConvertCharacter(m[cursor]);

        if ((cursor + 1) % MEMORY_WIDTH_EACHLEN == 0 ||
            cursor + 1 == len) 
        {
            ret.append(1, '\t');
            ret.append(af);
            ret.append(1, '\t');
            ret.append(df);

            if (cursor + 1 == len) {
                int pendingsize = 3 * (MEMORY_WIDTH_EACHLEN - cursor % MEMORY_WIDTH_EACHLEN);
                ret.append(pendingsize, ' ');
            }

            ret.append(1, '\t');
            ret.append(sf);
            ret.append(1, '\n');

            memset(df, 0, sizeof(df));
            memset(af, 0, sizeof(af));
            memset(sf, 0, sizeof(sf));
        }
        cursor++;
    }

    return ret;
}

void Logger::PrintMemory(LogLevels level, const std::string& filename, 
    const std::string& function, int linenum, const std::string& memory_name, 
    const void* memory, size_t len)
{
    if (level < loginfo_.level) {
        return;
    }
    std::string column_str = fmt_->Format(level, filename, function, linenum);
    std::string visualdata = BuildMemoryVisualData(memory, len);

    if (lnm_.NeedUpdate()) {
        printer_->SetIO(lnm_.GetLogName());
    }

    std::string finalstring = column_str + "###  Memory of " + memory_name + ":\n" + visualdata;

    printer_->Output(finalstring + "\n", level);
}



                