

#ifndef _LOGGER_H_
#define _LOGGER_H_


#include <string>
#include <thread>
#include <mutex>
#include <memory>

#include "logger_define.h"
#include "helper/logname.h"

typedef struct
{
    char        label[256];             // 用于做标识，可指定为任意字符串，此字符串将体现在日志的文件名上
    LogLevels   level;                  // 小于level的日志将不会被打印
    RunModel    runModel;				// 同步输出或者异步输出
    int         columns;				// 选择输出哪些列，参考 enum LogColumns
    LogOutput   outputModel;			// 输出至文件或者终端
    uint8_t		keep_days;              // 日志保存时间（即随时清理从当天算，keep_days天前的所有日志。如果为0，则不清理任何日志）
}LogInitInfo;

class Formatter;
class PrinterInterface;
class LogClr;

using Printer = std::shared_ptr<PrinterInterface>;
using Fmter = std::shared_ptr<Formatter>;
using AutoCleaner = std::shared_ptr<LogClr>;

class Logger
{
protected:
    Logger();
public:
    ~Logger();

    static Logger* GetInstance();
    static bool IsInited();

    void Init(const LogInitInfo& info, const std::string& version, const std::string& path);
    void Uninit();

    void Print(LogLevels level, const std::string& filename, const std::string& function, int linenum, const char* format, ...);

private:
    void SetLoggerHeader();
private:

    std::mutex mutex_;
    LogInitInfo loginfo_;
    std::string version_ = "";
    std::string logpath_ = "";
    Printer printer_ = nullptr;
    Fmter   fmt_ = nullptr;
    AutoCleaner clr_ = nullptr;
    LogNameManager lnm_;
private:
    static bool isInited_;
};



/* some setting for struct LogInitInfo*/
const LogInitInfo g_logfile_info_ = {
	"DefaultNameOfLog", kDebugLevel, kSync, kAllColumn, kFileModel, 0
};
const LogInitInfo g_logconsole_info = {
	"DefaultNameOfLog", kDebugLevel, kSync, kAllColumn, kConsoleModel, 0
};


#ifdef _ENABLE_LOGGER_

#define LOG_InitWithFileMode(ver, logdir)		Logger::GetInstance()->Init(g_logfile_info_, (ver), (logdir));
#define LOG_InitWithConsoleMode(ver, logdir) 	Logger::GetInstance()->Init(g_logconsole_info, (ver), (logdir));

#define LOG_Debug(format, ...)      		Logger::GetInstance()->Print(kDebugLevel, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define LOG_Infor(format, ...)      		Logger::GetInstance()->Print(kInforLevel, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define LOG_Warning(format, ...)    		Logger::GetInstance()->Print(kWarningLevel, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define LOG_Error(format, ...)      		Logger::GetInstance()->Print(kErrorLevel, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define LOG_Fatal(format, ...)      		Logger::GetInstance()->Print(kFatalLevel, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)

#else

#define LOG_InitWithFileMode(ver, logdir)
#define LOG_InitWithConsoleMode(ver, logdir)

#define LOG_Debug(format, ...)
#define LOG_Infor(format, ...)
#define LOG_Warning(format, ...)
#define LOG_Error(format, ...)
#define LOG_Fatal(format, ...)


#endif // _ENABLE_LOGGER_




#endif // _LOGGER_H_


