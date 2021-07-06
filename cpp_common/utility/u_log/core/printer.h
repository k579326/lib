#ifndef _PRINTER_H_
#define _PRINTER_H_


#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <map>

#include "logger_define.h"
#include <core/io.h>

class PrinterInterface
{
    using LogColorMap = std::map<LogLevels, Color>;

protected:
    PrinterInterface() {
        color_map_.insert(std::make_pair(kDebugLevel, White));
        color_map_.insert(std::make_pair(kInforLevel, Green));
        color_map_.insert(std::make_pair(kWarningLevel, Yellow));
        color_map_.insert(std::make_pair(kErrorLevel, Purple));
        color_map_.insert(std::make_pair(kFatalLevel, Red));
    }
public:
    virtual ~PrinterInterface() {}
    
    virtual void SetIO(const std::string& filename) = 0;
    virtual void Output(const std::string& str, LogLevels loglevel) = 0;

    Color GetColor(LogLevels loglevel) 
    {
        auto it = color_map_.find(loglevel);
        if (it == color_map_.end()) {
            return InvalidColor;
        }
        return it->second;
    }

private:
    LogColorMap color_map_;
};


class ThreadPrinter : public PrinterInterface
{
public:
    ThreadPrinter(LogOutput mode);
    ~ThreadPrinter();
    
    virtual void SetIO(const std::string& filename) override;
    virtual void Output(const std::string& str, LogLevels loglevel) override;
private:

    void Stop();
    void Start();
    void log_handler();

private:
    std::condition_variable cond_;
    std::mutex protector_;
    std::queue<std::tuple<std::string, LogLevels>> log_queue_;
    std::thread log_thread_;

    bool is_stop_ = false;
    bool is_exit_ = false;
    std::shared_ptr<IOInterface> io_ = nullptr;
};

class SyncPrinter : public PrinterInterface
{
public:
    SyncPrinter(LogOutput mode);
    ~SyncPrinter();
    
    /* 重新设置IO，内部有锁 */
    virtual void SetIO(const std::string& filename) override;

    /* 同步写入，内部有锁，控制外部多线程调用时的时序 */
    virtual void Output(const std::string& str, LogLevels loglevel) override;

private:
    std::shared_ptr<IOInterface> io_ = nullptr;
    std::mutex io_lock_;
};



#endif // _PRINTER_H_

