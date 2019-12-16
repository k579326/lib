

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
    RunModel    runModel;
    int         columns;
    LogOutput   outputModel;
}LogInitInfo;

class Formatter;
class PrinterInterface;

using Printer = std::shared_ptr<PrinterInterface>;
using Fmter = std::shared_ptr<Formatter>;

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
    LogNameManager lnm_;
private:
    static bool isInited_;
};


#define LOG_Debug(format, ...)      Logger::GetInstance()->Print(kDebugLevel, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define LOG_Infor(format, ...)      Logger::GetInstance()->Print(kInforLevel, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define LOG_Warning(format, ...)    Logger::GetInstance()->Print(kWarningLevel, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define LOG_Error(format, ...)      Logger::GetInstance()->Print(kErrorLevel, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define LOG_Fatal(format, ...)      Logger::GetInstance()->Print(kFatalLevel, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)





#endif


