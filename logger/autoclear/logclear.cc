

#include "logclear.h"

#include <chrono>

#include "u_dir/util_dir.h"
#include "u_path/util_path.h"


LogClr::LogClr(const std::string& log_dir, uint8_t keep_days, const LogNameManager* lm) : lm_(lm)
{
    stop_ = true;
    exit_ = false;
    keep_days_ = keep_days;
    interval_ = 5;
    log_dir_ = log_dir;

    thread_ = std::move(std::thread(&LogClr::ClearProc, this));
}

LogClr::~LogClr()
{
    exit_ = true;

    cond_.notify_one();
    thread_.join();
}

void LogClr::Start(uint8_t interval)
{
    std::unique_lock<std::mutex> autolock(mtx_);
    interval_ = interval;
    stop_ = false;

    cond_.notify_one();
}
void LogClr::Stop()
{
    stop_ = true;   // 这个就不用保护了，只有true和false
    cond_.notify_one();
}



void LogClr::ClearProc()
{
    size_t t = 0;

    while (true)
    {
        std::unique_lock<std::mutex> autolock(mtx_);
        std::cv_status status = cond_.wait_for(autolock, std::chrono::seconds(interval_));
        if (status == std::cv_status::timeout) {
            // do nothing
        }

        if (exit_) {
            break;
        }

        if (stop_) {
            continue;
        }

        RemoveLogTimeout();
    }           
    
    return;
}

void LogClr::RemoveLogTimeout()
{
    DIR* dp = opendir(log_dir_.c_str());
    if (dp == nullptr) {
        return;
    }
    
    auto is_timeout = [this](const std::string& filename)->bool
    {
        std::chrono::system_clock::time_point tp = lm_->GetDateOfLogName(filename);
        std::chrono::seconds nums = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - tp);
        
        return nums.count() >= keep_days_ * 24 * 3600;
    };


    while (dp != nullptr)
    {
        dirent* e = readdir(dp);
        if (e == nullptr) {
            break;
        }
        
        if (strcmp(e->d_name, ".") == 0 ||
            strcmp(e->d_name, "..") == 0)
        {
            continue;
        }
        
        std::string fullname = pathutil::PathCombines(log_dir_, e->d_name);
        if (e->d_type == DT_DIR) {
            // 日志目录下不应该有子目录
            CommRemoveDir(fullname.c_str(), true);
            continue;
        }

        if (is_timeout(e->d_name))
        {
            remove(fullname.c_str());
        }
    }
    
    closedir(dp);
    dp = nullptr;

    return;
}



