
#ifndef _LOG_CLEAR_H_
#define _LOG_CLEAR_H_

#include <stdint.h>


#include <thread>
#include <condition_variable>
#include <mutex>
#include <memory>

#include "helper/logname.h"


class LogClr
{
public:
    // keep_days: 保留指定天数内的日志，如果为0，将不清理日志
    LogClr(const std::string& log_dir, uint8_t keep_days, const LogNameManager* lm);
    ~LogClr();
    
    // interval: 清理线程的检查间隔，建议5秒
    void Start(uint8_t interval);
    void Stop();
private:
    
    void ClearProc();
    void RemoveLogTimeout();


private:

    std::string log_dir_;
    uint8_t keep_days_;
    uint8_t interval_;
    bool exit_;
    bool stop_;
    std::thread thread_;
    std::mutex mtx_;
    std::condition_variable cond_;
    
    const LogNameManager* lm_;
};



































#endif // _LOG_CLEAR_H_
