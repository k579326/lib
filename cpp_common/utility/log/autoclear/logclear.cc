

#include "logclear.h"

#include <string.h>

#include <chrono>

#include "dir/util_dir.h"
#include "path/util_path.h"
#if defined WIN32 || defined _WIN32
#include <windows.h>
#endif


LogClr::LogClr(const CptString& log_dir, uint8_t keep_days, const LogNameManager* lm) : lm_(lm)
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
    
    auto is_timeout = [this](const CptString& filename)->bool
    {
        // 为0时不清理
        if (keep_days_ == 0) {
            return false;
        }

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
        
        if (CptStringCmp(e->d_name, TEXT(".")) == 0 ||
            CptStringCmp(e->d_name, TEXT("..")) == 0)
        {
            continue;
        }
        
        CptString fullname = pathutil::PathCombines(log_dir_, e->d_name);
        if (DT_DIR == e->d_type) {
            // 日志目录下不应该有子目录
            CommRemoveDir(fullname.c_str(), true);
            continue;
        }

        if (is_timeout(e->d_name))
        {
#if defined WIN32 || defined _WIN32
            DeleteFile(fullname.c_str());
#else
            remove(fullname.c_str());
#endif
        }
    }
    
    closedir(dp);
    dp = nullptr;

    return;
}



