

#include "util_threadpool.h"

#include <algorithm>

namespace threads
{

ThreadPool* ThreadPool::ptr_ = nullptr;

ThreadPool* ThreadPool::CreateInstance(size_t size)
{
    static ThreadPool inst;
    inst.init(size);
    ptr_ = &inst;
    return ptr_;
}

ThreadPool* ThreadPool::GetInstance()
{
    return ptr_;
}


ThreadPool::ThreadPool()
{
    exit_ = true;
}

ThreadPool::~ThreadPool() 
{
    CancelAll();
}

int ThreadPool::init(size_t size)
{
    if (size > max_size_)
        size = max_size_;

    exit_ = false;
    pool_.reserve(size);
    for (int i = 0; i < size && pool_.size() < size; i++)
        pool_.push_back(std::thread(std::bind(&ThreadPool::_ThreadProcess, this)));

    return 0;
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
            cond_.wait(lock, [this]()->bool { 
                return !queue_.empty() || _IsQuit(); });
            
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
    exit_.compare_exchange_strong(expected, true);
    return expected;
}


void ThreadPool::CancelAll()
{
    {
        std::unique_lock<std::mutex> lock(mutex_);

        while (true)
        {
            if (queue_.empty())
            {
                break;
            }
            else {
                queue_.pop();
            }
        }
    }

    exit_ = true;
    cond_.notify_all();

    std::for_each(pool_.begin(), pool_.end(), [](std::thread& t)->void {  if (t.joinable()) t.join(); });
}















};


