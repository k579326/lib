#ifndef _UTIL_THREADPOOL_H_
#define _UTIL_THREADPOOL_H_


#include <assert.h>


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

template<class T>
using ThreadProc = std::function<void(std::shared_ptr<T>)>;

template<class T>
using FinCallback = std::function<void(std::shared_ptr<T>)>;

using _ThreadProc = std::function<void(void)>;
using _FinCallBack = std::function<void(void)>;

class ThreadPool
{
public:    
    ThreadPool(size_t size);
    ~ThreadPool();
    
    template<class T>
    void PushWorkQueue(ThreadProc<T> proc, std::shared_ptr<T> param, FinCallback<T> fin);

    template<class T>
    void PushWorkQueue(ThreadProc<T> proc);

private:

    bool _IsQuit();
    void _ThreadProcess();

private:    
    const int max_size_ = 16;
    
    std::queue<std::pair<_ThreadProc, _FinCallBack>> queue_;
    std::vector<std::thread>    pool_;


    std::atomic<bool>           exit_ = true;
    std::mutex                  mutex_;
    std::condition_variable     cond_;
};


template<class T>
void ThreadPool::PushWorkQueue(ThreadProc<T> proc, std::shared_ptr<T> param, FinCallback<T> fin)
{
    assert(proc);
    _ThreadProc tf = std::bind(proc, param);
    _FinCallBack ff = []()->void {}; 

    if (fin)
    {
        ff = std::bind(fin, param);
    }


    std::lock_guard<std::mutex> lock(mutex_);

    queue_.push(std::make_pair(tf, ff));
    cond_.notify_one();
}

template<class T>
void ThreadPool::PushWorkQueue(ThreadProc<T> proc)
{
    PushWorkQueue<T>(proc, nullptr, nullptr);
}


};
































#endif // _UTIL_THREADPOOL_H_


