
#ifndef _LOG_CLEAR_H_
#define _LOG_CLEAR_H_

#include <stdint.h>


#include <thread>
#include <condition_variable>
#include <mutex>
#include <memory>

#include "helper/logname.h"
#include "cpt-string.h"

/* 
*   bug记录：
* 
    此类在Windows 7平台存在bug, 在用MT方式编译代码后，程序退出时会卡死在日志清理线程中，
    在StackOverflow上有同样的issue(https://stackoverflow.com/questions/60452341/stdconditional-variable-notify-problem-on-windows-7-called-from-destructor-o)
    此问题暂时没有找到解决方案，放弃stl用boost又不现实，只能修改代码逻辑了.

    修改方案：不再启动清理线程，改为log初始化时，清理一次
*/

class LogClr
{
public:
    // keep_days: 保留指定天数内的日志，如果为0，将不清理日志
    LogClr(const CptString& log_dir, uint8_t keep_days, const LogNameManager* lm);
    ~LogClr();
    
    // interval: 清理线程的检查间隔，建议5秒
    void Start(uint8_t interval);
    void Stop();
private:
    
    void ClearProc();
    void RemoveLogTimeout();


private:

    CptString log_dir_;
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
