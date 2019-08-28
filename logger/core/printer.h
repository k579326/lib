#ifndef _PRINTER_H_
#define _PRINTER_H_


#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

class PrinterInterface
{
protected:
    PrinterInterface() {}
public:
    virtual ~PrinterInterface() {}
    
    virtual void Output(const std::string& str) = 0;
    
};


class ThreadPrinter : public PrinterInterface
{
public:
    ThreadPrinter();
    ~ThreadPrinter();
    
    virtual void Output(const std::string& str);
private:
    std::condition_variable cond_;
    std::mutex protector_;
    std::queue<std::string> log_queue_;
    std::thread log_thread_;
};

class SyncPrinter : public PrinterInterface
{
public:
    SyncPrinter();
    ~SyncPrinter();

    virtual void Output(const std::string& str);
};



#endif
