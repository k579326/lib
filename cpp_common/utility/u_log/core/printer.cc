

#include "printer.h"


ThreadPrinter::ThreadPrinter(LogOutput mode)
{
    io_ = UniversalFactory::GetInstance()->CreateIO(mode);
    log_thread_ = std::thread(&ThreadPrinter::log_handler, this);
}

ThreadPrinter::~ThreadPrinter()
{
    {
        std::unique_lock<std::mutex> autolock(protector_);
        
        is_exit_ = true;
        
        while (log_queue_.size() > 0) {
            log_queue_.pop();
        }
        
        cond_.notify_one();
    }
    log_thread_.join();
}


void ThreadPrinter::Output(_LOGPROPERTY& property)
{
    std::unique_lock<std::mutex> autolock(protector_);
    property.logstr += "\n";
    log_queue_.push(property);
    cond_.notify_one();
}

void ThreadPrinter::SetIO(const std::string& path)
{
    std::unique_lock<std::mutex> autolock(protector_);
    Stop();
    io_->Close();
    io_->Open(path);
    Start();
}

void ThreadPrinter::log_handler()
{
    while (!is_exit_)
    {
        _LOGPROPERTY log;

        std::unique_lock<std::mutex> autolock(protector_);
        cond_.wait(autolock, [this]()->bool
        {
            return is_exit_ || (!log_queue_.empty() && !is_stop_);
        });

        if (is_exit_ ) {
            continue;
        }

        if (!log_queue_.empty()) {
            log = log_queue_.front();
            log_queue_.pop();
        }

        if (io_) {
            io_->Write(log.logstr, GetColor(log));
        }
    }
}
void ThreadPrinter::Stop()
{
    is_stop_ = true;
}
void ThreadPrinter::Start()
{
    is_stop_ = false;
}



SyncPrinter::SyncPrinter(LogOutput mode)
{
    io_ = UniversalFactory::GetInstance()->CreateIO(mode);
}
SyncPrinter::~SyncPrinter()
{
    std::unique_lock<std::mutex> autolock(io_lock_);
    io_->Close();
}
void SyncPrinter::Output(_LOGPROPERTY& property)
{
    std::unique_lock<std::mutex> autolock(io_lock_);
    property.logstr += "\n";
    io_->Write(property.logstr, GetColor(property));
}
void SyncPrinter::SetIO(const std::string& path)
{
    std::unique_lock<std::mutex> autolock(io_lock_);
    io_->Close();
    io_->Open(path);
}



