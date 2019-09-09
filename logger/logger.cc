
#include "logger.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <thread>
#include <mutex>

#include "core/printer.h"

using Printer = std::shared_ptr<PrinterInterface>;

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

    std::mutex mutex_;
    LogInitInfo loginfo_;
    std::string version_ = "";
    std::string logpath_ = "";
    Printer printer_ = nullptr;

private:
    static bool isInited_;
};



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
        printer_.reset(new ThreadPrinter());
    }
    else
    {
        printer_.reset(new SyncPrinter());
    }

    isInited_ = true;
}

void Logger::Uninit()
{
    memset(&loginfo_, 0, sizeof(loginfo_));
    logpath_ = "";
    version_ = "";
}

void Logger::Print(LogLevels level, const std::string& filename, const std::string& function, int linenum, const char* format, ...)
{
    if (level < loginfo_.level) { 
        return ;
    }


}





















int LogInit(const LogInitInfo& info, const char* version, const char* path)
{
    std::string local_version;

    if (!path) {
        return -1;
    }
    if (!version) { 
        local_version = "";
    }

    Logger::GetInstance()->init(info, local_version, path);

    return 0;
}
            
void _log_print_(LogLevels level, 
                const char* filename, 
                const char* function, 
                int linenum, 
                const char* format, ...)
{
    if (!Logger::GetInstance()->IsInited()) {
        return;
    }

    assert(filename && function);


}
                
                
                