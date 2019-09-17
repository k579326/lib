


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

void ThreadPool::_ThreadProcess()
{
    while (true)
    {
        if (_IsQuit())
        {
            break;
        }

        std::pair<_ThreadProc, _FinCallBack> f;

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

        if (f.first != nullptr) f.first();
        if (f.second != nullptr) f.second();
    }

}

bool ThreadPool::_IsQuit()
{
    bool expected = true;
    return exit_.compare_exchange_strong(expected, true);
}



};


