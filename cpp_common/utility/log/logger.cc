
#include "logger.h"

#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "path/util_path.h"
#include "dir/util_dir.h"

#include "core/formatter.h"
#include "core/printer.h"
#include "autoclear/logclear.h"
#include "stringdef/stringdef.h"

static const CptChar* g_log_start = TEXT("\n\
**********************************************************\n\
**** Logger Startup Header                             \n\
**** Time:      %4d-%02d-%02d %02d:%02d:%02d           \n\
**** LogLevel:  %s                                     \n\
**** RunModel:  %s                                     \n\
**********************************************************\n\
");


#define USERLOG_MAXLENGTH   256
#define PATHBUF_MAXLENGTH   512

std::atomic<bool> Logger::isInited_(false);

Logger* Logger::GetInstance()
{
    static Logger logger;
    return &logger;
}


Logger::Logger()
{
    memset(&loginfo_, 0, sizeof(loginfo_));
}
Logger::~Logger()
{
}

bool Logger::IsInited()
{
    return isInited_;
}


void Logger::Init(const LogInitInfo& info, const CptString& version, const CptString& path)
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

    fmt_ = std::make_shared<Formatter>(info.columns, version);

    if (info.outputModel == kFileModel) 
    {
        CptString logpath;
        logpath = pathutil::RelativeToAbsolute(path);
        if (logpath.empty()) {
            assert(false);
            return;
        }
        logpath = pathutil::PathsCombine<CptString>(logpath, TEXT("logs"));
        lnm_.SetLabel(info.label);
        lnm_.SetPath(logpath);

        CommCreateDir(logpath.c_str());
        logpath_ = logpath;
        clr_ = std::make_shared<LogClr>(logpath_, info.keep_days, &lnm_);
        clr_->Start(5); // 5秒检查一次
    }
    else {
        logpath_ = TEXT("");
    }

    loginfo_ = info;
    version_ = version;

    printer_->SetIO(lnm_.GetLogName());
    SetLoggerHeader();

    isInited_ = true;
}

void Logger::SetLoggerHeader()
{
    CptChar buf[1024];

    time_t timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    tm* tp = localtime(&timestamp);
    CptStringPrintf(buf, g_log_start, tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday,
        tp->tm_hour, tp->tm_min, tp->tm_sec, 
        g_loglevel_str.find(loginfo_.level)->second.c_str(), 
        g_runmodel_str.find(loginfo_.runModel)->second.c_str()
    );

    _LOGPROPERTY log;
    log.logstr = buf;
    log.level = kInforLevel;
    printer_->Output(log);
}


void Logger::Uninit()
{
    if (!IsInited())
        return;
    // memset(&loginfo_, 0, sizeof(loginfo_));
    logpath_ = TEXT("");
    version_ = TEXT("");

    fmt_.reset();
    clr_.reset();
    printer_.reset();

    isInited_ = false;
}

void Logger::Print(uint8_t floor, TextColor tc, const CptChar* format, ...)
{
    if (tc > TC_NOUSE) {
        tc = TextColor::TC_White;
    }

    CptChar buf[USERLOG_MAXLENGTH] = { 0 };
    va_list varArgs;
    va_start(varArgs, format);
    Cptvsnprintf(buf, USERLOG_MAXLENGTH, format, varArgs);
    
    va_end(varArgs);

    if (lnm_.NeedUpdate()) {
        printer_->SetIO(lnm_.GetLogName());
    }
    
    CptString logstr((size_t)floor * 2, TEXT('\x20'));
    logstr += buf;

    _LOGPROPERTY log;
    log.logstr = logstr;
    log.level = kLevelEnd;
    log.color = tc;
    printer_->Output(log);
}

void Logger::Print(LogLevels level, const CptString& filename,
    const CptString& function, int linenum, const CptChar* format, ...)
{
    if (level < loginfo_.level) { 
        return ;
    }

    CptString column_str = fmt_->Format(level, filename, function, linenum);

    CptChar buf[USERLOG_MAXLENGTH];
    va_list varArgs;
    va_start(varArgs, format);
    Cptvsnprintf(buf, USERLOG_MAXLENGTH, format, varArgs);
    va_end(varArgs);

    if (lnm_.NeedUpdate()) {
        printer_->SetIO(lnm_.GetLogName());
    }

    _LOGPROPERTY log;
    log.logstr = column_str + TEXT("###  ") + buf;
    log.level = level;

    printer_->Output(log);
    return;
}

CptChar Logger::ConvertCharacter(const char& c)
{
    if ((uint8_t)c > 0x7eu || (uint8_t)c < 0x20u) {
        if ((uint8_t)c > 0x7eu)
            return TEXT('?');
        else
            return TEXT('.');
    }

    return c;
}

CptString Logger::BuildMemoryVisualData(const void* memory, size_t len)
{
    CptString ret;
    // mast large than (strlen(df) + strlen(af) + strlen(sf))
    ret.reserve(5 * len);

    if (MEMORY_WIDTH_EACHLEN < 8) {
        // bad setting
        return TEXT("");
    }

    size_t cursor = 0;
    char* m = reinterpret_cast<char*>(const_cast<void*>(memory));
    CptChar df[MEMORY_WIDTH_EACHLEN * 3 + 1] = { 0 };
    CptChar af[32] = {}, sf[MEMORY_WIDTH_EACHLEN + 1] = {};
    while (cursor < len)
    {
        if (cursor % MEMORY_WIDTH_EACHLEN == 0) {
            CptStringPrintf(af, TEXT("%p"), m + cursor);
        }

        CptChar byte[4];
        CptStringPrintf(byte, TEXT("%02x "), (uint8_t)m[cursor]);
        CptStringCat(df, byte);

        sf[cursor % MEMORY_WIDTH_EACHLEN] = ConvertCharacter(m[cursor]);

        if ((cursor + 1) % MEMORY_WIDTH_EACHLEN == 0 ||
            cursor + 1 == len) 
        {
            ret.append(1, TEXT('\t'));
            ret.append(af);
            ret.append(1, TEXT('\t'));
            ret.append(df);

            if (cursor + 1 == len) {
                int pendingsize = 3 * (MEMORY_WIDTH_EACHLEN - cursor % MEMORY_WIDTH_EACHLEN);
                ret.append(pendingsize, TEXT(' '));
            }

            ret.append(1, TEXT('\t'));
            ret.append(sf);
            ret.append(1, TEXT('\n'));

            memset(df, 0, sizeof(df));
            memset(af, 0, sizeof(af));
            memset(sf, 0, sizeof(sf));
        }
        cursor++;
    }

    return ret;
}

void Logger::PrintMemory(LogLevels level, const CptString& filename,
    const CptString& function, int linenum, const CptString& memory_name,
    const void* memory, size_t len)
{
    if (level < loginfo_.level) {
        return;
    }
    CptString column_str = fmt_->Format(level, filename, function, linenum);
    CptString visualdata = BuildMemoryVisualData(memory, len);

    if (lnm_.NeedUpdate()) {
        printer_->SetIO(lnm_.GetLogName());
    }

    _LOGPROPERTY log;
    log.logstr = column_str + TEXT("###  Memory of ") + memory_name + TEXT(":\n") + visualdata;
    log.level = level;

    printer_->Output(log);
}



                
