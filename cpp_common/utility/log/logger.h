

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
#include "cpt-string.h"

typedef struct
{
    CptString   label;             // 用于做标识，可指定为任意字符串，此字符串将体现在日志的文件名上
    LogLevels   level;                  // 小于level的日志将不会被打印
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

    void Init(const LogInitInfo& info, const CptString& version, const CptString& path);
    void Uninit();

    /* 
    *   floor:  means indent，hierarchically log have no indent default.
        tc:     valid for console model only 
    */
    void Print(uint8_t floor, TextColor tc, const CptChar* format, ...);

    void Print(LogLevels level, const CptString& filename,
        const CptString& function, int linenum, const CptChar* format, ...);
    void PrintMemory(LogLevels level, const CptString& filename,
        const CptString& function, int linenum, const CptString& memory_name,
        const void* memory, size_t len);

private:

    void SetLoggerHeader();
    CptChar ConvertCharacter(const char& c);
    CptString BuildMemoryVisualData(const void* memory, size_t len);

private:

    std::mutex mutex_;
    LogInitInfo loginfo_;
    CptString version_;
    CptString logpath_;
    Printer printer_ = nullptr;
    Fmter   fmt_ = nullptr;
    AutoCleaner clr_ = nullptr;
    LogNameManager lnm_;
private:
    static std::atomic<bool> isInited_;
};



/* some setting for struct LogInitInfo*/
static LogInitInfo g_logfile_info_ = {
	TEXT("DefaultNameOfLog"), kDebugLevel, kAllColumn, kFileModel, 0
};
static LogInitInfo g_logconsole_info = {
	TEXT("DefaultNameOfLog"), kDebugLevel, kAllColumn, kConsoleModel, 0
};


#ifdef _ENABLE_LOGGER_

#define LOG_InitAsFileMode(label_str, ver, logdir)		\
	g_logfile_info_.label = label_str;					\
	Logger::GetInstance()->Init(g_logfile_info_, (ver), (logdir))
	
#define LOG_InitAsConsoleMode(label_str, ver, logdir) 	\
	g_logconsole_info.label = label_str;	\
	Logger::GetInstance()->Init(g_logconsole_info, (ver), (logdir))

#define LOG_Uninit()    \
    Logger::GetInstance()->Uninit()


#define _LOG_FILE_NAME_ MACROSTRINGEXPAND(__FILE__)
#define _LOG_FUNC_NAME_ MACROSTRINGEXPAND(__FUNCTION__)


#define LOG_Debug(format, ...)      		Logger::GetInstance()->Print(kDebugLevel, _LOG_FILE_NAME_, _LOG_FUNC_NAME_, __LINE__, format, ##__VA_ARGS__)
#define LOG_Infor(format, ...)      		Logger::GetInstance()->Print(kInforLevel, _LOG_FILE_NAME_, _LOG_FUNC_NAME_, __LINE__, format, ##__VA_ARGS__)
#define LOG_Warning(format, ...)    		Logger::GetInstance()->Print(kWarningLevel, _LOG_FILE_NAME_, _LOG_FUNC_NAME_, __LINE__, format, ##__VA_ARGS__)
#define LOG_Error(format, ...)      		Logger::GetInstance()->Print(kErrorLevel, _LOG_FILE_NAME_, _LOG_FUNC_NAME_, __LINE__, format, ##__VA_ARGS__)
#define LOG_Fatal(format, ...)      		Logger::GetInstance()->Print(kFatalLevel, _LOG_FILE_NAME_, _LOG_FUNC_NAME_, __LINE__, format, ##__VA_ARGS__)

#define LOGM_Debug(name, addr, len)      		Logger::GetInstance()->PrintMemory(kDebugLevel, _LOG_FILE_NAME_, _LOG_FUNC_NAME_, __LINE__, (name), (addr), (len))
#define LOGM_Infor(name, addr, len)      		Logger::GetInstance()->PrintMemory(kInforLevel, _LOG_FILE_NAME_, _LOG_FUNC_NAME_, __LINE__, (name), (addr), (len))
#define LOGM_Warning(name, addr, len)    		Logger::GetInstance()->PrintMemory(kWarningLevel, _LOG_FILE_NAME_, _LOG_FUNC_NAME_, __LINE__, (name), (addr), (len))
#define LOGM_Error(name, addr, len)      		Logger::GetInstance()->PrintMemory(kErrorLevel, _LOG_FILE_NAME_, _LOG_FUNC_NAME_, __LINE__, (name), (addr), (len))
#define LOGM_Fatal(name, addr, len)      		Logger::GetInstance()->PrintMemory(kFatalLevel, _LOG_FILE_NAME_, _LOG_FUNC_NAME_, __LINE__, (name), (addr), (len))

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


