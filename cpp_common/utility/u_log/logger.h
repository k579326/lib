

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <string>
#include <thread>
#include <mutex>
#include <memory>
#include <atomic>

#include "logger_define.h"
#include "helper/logname.h"

typedef struct
{
    char        label[256];             // 用于做标识，可指定为任意字符串，此字符串将体现在日志的文件名上
    LogLevels   level;                  // 小于level的日志将不会被打印
    RunModel    runModel;				// 同步输出或者异步输出
    int         columns;				// 选择输出哪些列，参考 enum LogColumns
    LogOutput   outputModel;			// 输出至文件或者终端
    uint8_t		keep_days;              // 日志保存时间（即随时清理->从当天算keep_days天前的所有日志。如果为0，则不清理任何日志）
}LogInitInfo;

class Formatter;
class PrinterInterface;
class LogClr;

using Printer = std::shared_ptr<PrinterInterface>;
using Fmter = std::shared_ptr<Formatter>;
using AutoCleaner = std::shared_ptr<LogClr>;

class Logger
{
// 输出内存数据时，每行显示的内存字节数
#define MEMORY_WIDTH_EACHLEN    16

protected:
    Logger();
public:
    ~Logger();

    static Logger* GetInstance();
    inline static bool IsInited();

    void Init(const LogInitInfo& info, const std::string& version, const std::string& path);
    void Uninit();

    /* 
    *   floor:  means indent，hierarchically log have no indent default.
        tc:     valid for console model only 
    */
    void Print(uint8_t floor, TextColor tc, const char* format, ...);

    void Print(LogLevels level, const std::string& filename,
        const std::string& function, int linenum, const char* format, ...);
    void PrintMemory(LogLevels level, const std::string& filename, 
        const std::string& function, int linenum, const std::string& memory_name, 
        const void* memory, size_t len);

private:

    void SetLoggerHeader();
    char ConvertCharacter(const char& c);
    std::string BuildMemoryVisualData(const void* memory, size_t len);

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
    static std::atomic<bool> isInited_;
};



/* some setting for struct LogInitInfo*/
static LogInitInfo g_logfile_info_ = {
	"DefaultNameOfLog", kDebugLevel, kSync, kAllColumn, kFileModel, 0
};
static LogInitInfo g_logconsole_info = {
	"DefaultNameOfLog", kDebugLevel, kSync, kAllColumn, kConsoleModel, 0
};


#ifdef _ENABLE_LOGGER_

#define LOG_InitAsFileMode(label_str, ver, logdir)		\
	strcpy(g_logfile_info_.label, label_str);					\
	Logger::GetInstance()->Init(g_logfile_info_, (ver), (logdir))
	
#define LOG_InitAsConsoleMode(label_str, ver, logdir) 	\
	strcpy(g_logconsole_info.label, label_str);	\
	Logger::GetInstance()->Init(g_logconsole_info, (ver), (logdir))

#define LOG_Uninit()    \
    Logger::GetInstance()->Uninit()

#define LOG_Debug(format, ...)      		Logger::GetInstance()->Print(kDebugLevel, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define LOG_Infor(format, ...)      		Logger::GetInstance()->Print(kInforLevel, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define LOG_Warning(format, ...)    		Logger::GetInstance()->Print(kWarningLevel, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define LOG_Error(format, ...)      		Logger::GetInstance()->Print(kErrorLevel, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define LOG_Fatal(format, ...)      		Logger::GetInstance()->Print(kFatalLevel, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)

#define LOGM_Debug(name, addr, len)      		Logger::GetInstance()->PrintMemory(kDebugLevel, __FILE__, __FUNCTION__, __LINE__, (name), (addr), (len))
#define LOGM_Infor(name, addr, len)      		Logger::GetInstance()->PrintMemory(kInforLevel, __FILE__, __FUNCTION__, __LINE__, (name), (addr), (len))
#define LOGM_Warning(name, addr, len)    		Logger::GetInstance()->PrintMemory(kWarningLevel, __FILE__, __FUNCTION__, __LINE__, (name), (addr), (len))
#define LOGM_Error(name, addr, len)      		Logger::GetInstance()->PrintMemory(kErrorLevel, __FILE__, __FUNCTION__, __LINE__, (name), (addr), (len))
#define LOGM_Fatal(name, addr, len)      		Logger::GetInstance()->PrintMemory(kFatalLevel, __FILE__, __FUNCTION__, __LINE__, (name), (addr), (len))

// color: enum type
#define LOG_FREE(floor, color, format, ...)     Logger::GetInstance()->Print(floor, color, format, ##__VA_ARGS__)

#else

#define LOG_InitAsFileMode(label_str, ver, logdir)
#define LOG_InitAsConsoleMode(label_str, ver, logdir)
#define LOG_Uninit()

#define LOG_Debug(format, ...)
#define LOG_Infor(format, ...)
#define LOG_Warning(format, ...)
#define LOG_Error(format, ...)
#define LOG_Fatal(format, ...)

#define LOGM_Debug(name, addr, len)    
#define LOGM_Infor(name, addr, len)    
#define LOGM_Warning(name, addr, len)  
#define LOGM_Error(name, addr, len)    
#define LOGM_Fatal(name, addr, len)    

#define LOG_FREE(floor, color, format, ...) 

#endif // _ENABLE_LOGGER_




#endif // _LOGGER_H_


