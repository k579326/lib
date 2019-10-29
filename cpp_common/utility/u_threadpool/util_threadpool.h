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

namespace threads
{

template<class T>
using ThreadProc = std::function<void(std::shared_ptr<T>)>;

template<class T>
using FinCallback = std::function<void(std::shared_ptr<T>)>;

using _ThreadProc = std::function<void(void)>;
using _FinCallBack = std::function<void(void)>;

class ThreadPool
{
protected:
    ThreadPool();
public:
    ~ThreadPool();
    
    static ThreadPool* CreateInstance(size_t size);
    static ThreadPool* GetInstance();

    int init(size_t size);

    template<class T>
    void PushWorkQueue(ThreadProc<T> proc, std::shared_ptr<T> param, FinCallback<T> fin);

    void PushWorkQueue(std::function<void(void)> proc);

    void CancelAll();

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

    static ThreadPool* ptr_;
};


template<class T>
inline void ThreadPool::PushWorkQueue(ThreadProc<T> proc, std::shared_ptr<T> param, FinCallback<T> fin)
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

inline void ThreadPool::PushWorkQueue(std::function<void(void)> proc)
{
    assert(proc);
    
    _FinCallBack ff = []()->void {}; 

    std::lock_guard<std::mutex> lock(mutex_);

    queue_.push(std::make_pair(proc, ff));
    cond_.notify_one();
}


};





#endif // _UTIL_THREADPOOL_H_


