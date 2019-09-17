#ifndef _UTIL_THREADPOOL_H_
#define _UTIL_THREADPOOL_H_

#include <functional>
#include <thread>
#include <atomic>
#include <memory>
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>

namespace util_threadpool
{

using ThreadProc = std::function<void(void)>;

class ThreadPool
{
public:    
    ThreadPool(size_t size);
    ~ThreadPool();
    
    void PushWorkQueue(ThreadProc f);
    
private:

    bool _IsQuit();
    void _ThreadProcess();

private:    
    const int max_size_ = 16;
    
    std::queue<ThreadProc>      queue_;
    std::vector<std::thread>    pool_;


    std::atomic<bool>           exit_ = true;
    std::mutex                  mutex_;
    std::condition_variable     cond_;
};


};
































#endif // _UTIL_THREADPOOL_H_


