


#include "util_threadpool.h"

namespace util_threadpool
{


ThreadPool::ThreadPool(size_t size)
{
    if (size > max_size_)
        size = max_size_;

    exit_ = false;
    pool_.reserve(size);
    pool_.push_back(std::thread(std::bind(&ThreadPool::_ThreadProcess, this)));
}

ThreadPool::~ThreadPool() 
{
    exit_ = true;
    cond_.notify_all();
    
    std::for_each(pool_.begin(), pool_.end(), [](std::thread& t)->void {  t.join(); });
}

void ThreadPool::PushWorkQueue(ThreadProc f)
{
    std::lock_guard<std::mutex> lock(mutex_);

    queue_.push(f);
    cond_.notify_one();
}



void ThreadPool::_ThreadProcess()
{
    while (true)
    {
        if (_IsQuit())
        {
            break;
        }

        ThreadProc f;

        {
            std::unique_lock<std::mutex> lock(mutex_);
            
            // µÈ´ý
            cond_.wait(lock, [this]()->bool { return !queue_.empty() || _IsQuit(); });
            if (queue_.empty())
            {
                continue;
            }

            f = queue_.front();
            queue_.pop();
        }

        f();
    }

}

bool ThreadPool::_IsQuit()
{
    bool expected = true;
    return exit_.compare_exchange_strong(expected, true);
}



};


