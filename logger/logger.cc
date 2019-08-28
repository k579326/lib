
#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <string>
#include <thread>
#include <mutex>



class Logger
{
protected:
    Logger();
public:    
    ~Logger();
    
    static Logger* GetInstance();
    static bool IsInited();

    void init(const LogInitInfo& info, const std::string& version, const std::string& path);
    void uninit();

private:

    std::mutex mutex_;
    LogInitInfo loginfo_;
    std::string version_{""};
    std::string logpath_{""};

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

void Logger::init(const LogInitInfo& info, const std::string& version, const std::string& path)
{
    if (isInited_)
    {
        return;
    }

    loginfo_ = info;
    version_ = version;
    logpath_ = path;

    isInited_ = true;
}

void Logger::uninit()
{

}





int LogInit(const LogInitInfo& info, const char* version, const char* path)
{
    
}
            
void _log_print_(LogLevels level, 
                const char* filename, 
                const char* function, 
                int linenum, 
                const char* format, ...)
{





}
                
                
                