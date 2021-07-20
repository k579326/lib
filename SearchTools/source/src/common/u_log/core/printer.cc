

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


void ThreadPrinter::Output(const std::string& str, LogLevels loglevel)
{
    std::unique_lock<std::mutex> autolock(protector_);

    log_queue_.push(std::tuple<std::string, LogLevels>(str, loglevel));
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
        std::tuple<std::string, LogLevels> logstr;

        std::unique_lock<std::mutex> autolock(protector_);
        cond_.wait(autolock, [this]()->bool
        {
            return is_exit_ || (!log_queue_.empty() && !is_stop_);
        });

        if (is_exit_ ) {
            continue;
        }

        if (!log_queue_.empty()) {
            logstr = log_queue_.front();
            log_queue_.pop();
        }

        if (io_)
            io_->Write(std::get<0>(logstr), GetColor(std::get<1>(logstr)));
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
void SyncPrinter::Output(const std::string& str, LogLevels loglevel)
{
    std::unique_lock<std::mutex> autolock(io_lock_);
    io_->Write(str, GetColor(loglevel));
}
void SyncPrinter::SetIO(const std::string& path)
{
    std::unique_lock<std::mutex> autolock(io_lock_);
    io_->Close();
    io_->Open(path);
}



