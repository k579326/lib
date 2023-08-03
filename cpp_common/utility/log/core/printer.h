#ifndef _PRINTER_H_
#define _PRINTER_H_


#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <map>

#include "cpt-string.h"
#include "logger_define.h"
#include <core/io.h>


struct _LOGPROPERTY
{
    _LOGPROPERTY() {
        logstr = TEXT("");
        level = kLevelEnd;
        color = TC_White;
    }
    CptString   logstr;
    LogLevels   level;
    TextColor   color;  // valid only level is kLevelEnd
};


class PrinterInterface
{
protected:
    PrinterInterface() {
    }
public:
    virtual ~PrinterInterface() {}
    
    virtual void SetIO(const CptString& filename) = 0;
    virtual void Output(_LOGPROPERTY& properties) = 0;
};

class SyncPrinter : public PrinterInterface
{
public:
    SyncPrinter(LogOutput mode);
    ~SyncPrinter();
    
    /* 重新设置IO，内部有锁 */
    virtual void SetIO(const CptString& filename) override;

    /* 同步写入，内部有锁，控制外部多线程调用时的时序 */
    virtual void Output(_LOGPROPERTY& property) override;

private:
    std::shared_ptr<IOInterface> io_ = nullptr;
    std::mutex io_lock_;
};



#endif // _PRINTER_H_

