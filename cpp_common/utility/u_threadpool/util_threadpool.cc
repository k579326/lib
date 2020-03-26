

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
    for (int i = 0; i < size && pool_.size() < size; i++) {
        std::shared_ptr<Thread> trd_ctx = std::make_shared<Thread>();
        pool_.push_back(trd_ctx);
		
		std::thread trd(std::bind(&ThreadPool::_ThreadProcess, this, trd_ctx));
        (*pool_.rbegin())->thread_ = std::move(trd);
    }

    idle_count_ = size;
    return 0;
}

bool ThreadPool::AddThreads(int count)
{
    if (count == 0)
        return true;
    if (count < 0)
        return DelThreads(-count);

    return AddThreads((size_t)count);
}


bool ThreadPool::AddThreads(size_t count)
{
    std::lock_guard<std::mutex> autolock(mutex_);

    if (count + idle_count_ > max_size_ || exit_)
        return false;
    
    for (size_t i = 0; i < count; i++) {
        std::shared_ptr<Thread> trd_ctx = std::make_shared<Thread>();
        pool_.push_back(trd_ctx);
		std::thread trd(std::bind(&ThreadPool::_ThreadProcess, this, trd_ctx));
        (*pool_.rbegin())->thread_ = std::move(trd);
    }
    idle_count_ += count;

    return true;
}

bool ThreadPool::DelThreads(size_t count)
{
    size_t delsize = 0;

    {
        std::lock_guard<std::mutex> autolock(mutex_);

        if (count > idle_count_ || exit_)
            return false;
    }

    std::vector<std::shared_ptr<Thread>> del_threads;

    mutex_.lock();
    for (; delsize < count; delsize++)
    {
        auto it = std::find_if(pool_.begin(), pool_.end(), 
            [](std::shared_ptr<Thread> ptr)->bool { return !ptr->is_working_; });
        if (it == pool_.end()) {
            // 异常，不应该会到这里
            assert(false);
            break;
        }
        
        (*it)->is_del_ = true;
        del_threads.push_back(*it);
        pool_.erase(it);
    }
    mutex_.unlock();


    // 条件变量没办法指定唤醒哪个线程，只能全部唤醒。
    cond_.notify_all();
    for (auto it = del_threads.begin(); it != del_threads.end(); it++)
    {
        if ((*it)->thread_.joinable())
            (*it)->thread_.join();
    }

    // 如果delsize != count, 把删除的线程再加回来
    if (delsize != count) {

        for (; delsize != 0; delsize--) {
            std::shared_ptr<Thread> trd_ctx = std::make_shared<Thread>();
            pool_.push_back(trd_ctx);
            std::thread trd(std::bind(&ThreadPool::_ThreadProcess, this, trd_ctx));
			(*pool_.rbegin())->thread_ = std::move(trd);
        }

        return false;
    }
    
    mutex_.lock();
    idle_count_ -= count;
    mutex_.unlock();

    return true;
}

size_t ThreadPool::GetThreadCount()
{
    std::lock_guard<std::mutex> autolock(mutex_);
    return pool_.size();
}



void ThreadPool::_ThreadProcess(std::shared_ptr<Thread> thread_ctx)
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
            
            // 等待
            cond_.wait(lock, [this, thread_ctx]()->bool {
                return !queue_.empty() || _IsQuit() || thread_ctx->is_del_ ; });

            if (thread_ctx->is_del_)
                break;

            if (queue_.empty())
                continue;

            thread_ctx->is_working_ = true;
            idle_count_--;

            f = queue_.front();
            queue_.pop();
        }

        if (f.first != nullptr) f.first();
        if (f.second != nullptr) f.second();

        mutex_.lock();
        thread_ctx->is_working_ = false;
        idle_count_++;
        mutex_.unlock();
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
        exit_ = true;
    }

    cond_.notify_all();

    std::for_each(pool_.begin(), pool_.end(), [](std::shared_ptr<Thread> ctx)->void 
    {  
        if (ctx->thread_.joinable()) 
            ctx->thread_.join(); 
    });

}















};


