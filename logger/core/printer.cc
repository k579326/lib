

#include "printer.h"


ThreadPrinter::ThreadPrinter(LogOutput mode)
{
    io_ = UniversalFactory::GetInstance()->CreateIO(mode);
    log_thread_ = std::thread(&ThreadPrinter::log_handler, this);
}

ThreadPrinter::~ThreadPrinter()
{
    std::unique_lock<std::mutex> autolock(protector_);
    
    is_exit_ = true;
    while (log_queue_.size() > 0) {
        log_queue_.pop();
    }

    cond_.notify_one(); 
    log_thread_.join();
}


void ThreadPrinter::Output(const std::string& str)
{
    std::unique_lock<std::mutex> autolock(protector_);

    log_queue_.push(str);
    cond_.notify_one();
}

void ThreadPrinter::SetIO(const std::string& path)
{
    Stop();
    // 等一秒应该能让一条日志写完了吧
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    io_->Close();
    io_->Open(path);
    Start();
}

void ThreadPrinter::log_handler()
{
    while (is_exit_)
    {
        std::string logstr;

        {
            std::unique_lock<std::mutex> autolock(protector_);
            cond_.wait(autolock, [this]()->bool 
            { 
                return is_exit_ || (!log_queue_.empty() && !is_stop_);
            });

            logstr = log_queue_.front();
            log_queue_.pop();
        }

        if (is_exit_ || logstr.empty()) {
            continue;
        }

        if (io_)
            io_->Write(logstr);
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
    io_->Close();
}
void SyncPrinter::Output(const std::string& str)
{
    std::unique_lock<std::mutex> autolock(io_lock_);
    io_->Write(str);
}
void SyncPrinter::SetIO(const std::string& path)
{
    std::unique_lock<std::mutex> autolock(io_lock_);
    io_->Close();
    io_->Open(path);
}



