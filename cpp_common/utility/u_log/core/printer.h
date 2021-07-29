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


struct _LOGPROPERTY
{
    _LOGPROPERTY() {
        logstr = "";
        level = kLevelEnd;
        color = TC_White;
    }
    std::string logstr;
    LogLevels   level;
    TextColor   color;  // valid only level is kLevelEnd
};


class PrinterInterface
{
    using LogColorMap = std::map<LogLevels, __LogTextColor>;
    using LogColorMapFormOut = std::map<TextColor, __LogTextColor>;

protected:
    PrinterInterface() {
        color_map_.insert(std::make_pair(kDebugLevel, White));
        color_map_.insert(std::make_pair(kInforLevel, Green));
        color_map_.insert(std::make_pair(kWarningLevel, Yellow));
        color_map_.insert(std::make_pair(kErrorLevel, Purple));
        color_map_.insert(std::make_pair(kFatalLevel, Red));

        out_color_map_.insert(std::make_pair(TC_White, White));
        out_color_map_.insert(std::make_pair(TC_Green, Green));
        out_color_map_.insert(std::make_pair(TC_Yellow, Yellow));
        out_color_map_.insert(std::make_pair(TC_Purple, Purple));
        out_color_map_.insert(std::make_pair(TC_Black, Black));
        out_color_map_.insert(std::make_pair(TC_Red, Red));
        out_color_map_.insert(std::make_pair(TC_Cyan, Cyan));
        out_color_map_.insert(std::make_pair(TC_Blue, Blue));
    }
public:
    virtual ~PrinterInterface() {}
    
    virtual void SetIO(const std::string& filename) = 0;
    virtual void Output(_LOGPROPERTY& properties) = 0;

    __LogTextColor GetColor(const _LOGPROPERTY& log)
    {
        __LogTextColor c;
        if (log.level == kLevelEnd) {
            auto it = out_color_map_.find(log.color);
            if (it == out_color_map_.end()) {
                return InvalidColor;
            }
            c = it->second;
        }
        else {
            auto it = color_map_.find(log.level);
            if (it == color_map_.end()) {
                return InvalidColor;
            }
            c = it->second;
        }
             
        return c;
    }

private:
    LogColorMap color_map_;
    LogColorMapFormOut out_color_map_;
};


class ThreadPrinter : public PrinterInterface
{
public:
    ThreadPrinter(LogOutput mode);
    ~ThreadPrinter();
    
    virtual void SetIO(const std::string& filename) override;
    virtual void Output(_LOGPROPERTY& property) override;
private:

    void Stop();
    void Start();
    void log_handler();

private:
    std::condition_variable cond_;
    std::mutex protector_;
    std::queue<_LOGPROPERTY> log_queue_;
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
    virtual void Output(_LOGPROPERTY& property) override;

private:
    std::shared_ptr<IOInterface> io_ = nullptr;
    std::mutex io_lock_;
};



#endif // _PRINTER_H_

