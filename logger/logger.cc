
#include "logger.h"

#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "stringdef/stringdef.h"

static const char* g_log_start = "\n\
**********************************************************\n\
**** Logger Startup Header                            \n\
**** Time: %4d-%02d-%02d %02d:%02d:%02d               \n\
**** LogLevel: %s                                     \n\
**** RunModel: %s                                     \n\
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

    loginfo_ = info;
    version_ = version;
    logpath_ = path;
    // init printer
    if (info.runModel == RunModel::kAsync) { 
        printer_.reset(new ThreadPrinter(info.outputModel));
    }
    else
    {
        printer_.reset(new SyncPrinter(info.outputModel));
    }

    fmt_ = std::make_shared<Formatter>(info.columns, version);
    lnm_.SetLabel(info.label);
    lnm_.SetPath(path);

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

    printer_->Output(buf);
}


void Logger::Uninit()
{
    memset(&loginfo_, 0, sizeof(loginfo_));
    logpath_ = "";
    version_ = "";

    isInited_ = false;
}

void Logger::Print(LogLevels level, const std::string& filename, const std::string& function, int linenum, const char* format, ...)
{
    if (level < loginfo_.level) { 
        return ;
    }

    std::string column_str = fmt_->Format(level, filename, function, linenum);

    char buf[4096] = { 0 };
    va_list varArgs;
    va_start(varArgs, format);
    std::vsnprintf(buf, 4096, format, varArgs);
    va_end(varArgs);

    if (lnm_.NeedUpdate()) {
        printer_->SetIO(lnm_.GetLogName());
    }

    printer_->Output(column_str + "###  " + buf + "\n");
}

          
                