
/* 
* c++11 信号量
* 
* author: guok
*/

#ifndef _CC_SEMAPHORE_H_
#define _CC_SEMAPHORE_H_

#include <assert.h>

#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>


namespace std
{
    class semaphore
    {
    public:
        semaphore(int size) : max_(size), idle_(size)
        {
        }
        ~semaphore()
        {
        }
        
        semaphore(const semaphore&) = delete;
        semaphore(semaphore&&) = delete;
        semaphore& operator=(const semaphore&) = delete;
        semaphore& operator==(semaphore&&) = delete;
        
        /* 占用一个资源，最多等待ms毫秒, 如果为-1则不返回 */
        bool wait(std::chrono::milliseconds ms)
        {
            std::unique_lock<std::mutex> auto_lock(lock_);

            if (idle_ == 0)
            {
                bool ret;
                ret = cond_.wait_for(auto_lock, ms, [this]()->bool { return idle_ > 0; });

                if (!ret)
                {
                    return false;
                }
            }

            idle_--;
            assert(idle_ >= 0);

            return true;
        }
        
        /* 获取到了信号量则返回true, 否则返回false */
        bool try_wait()
        {
            std::unique_lock<std::mutex> auto_lock(lock_);

            if (idle_ == 0)
            {
                return false;
            }

            cond_.wait(auto_lock, [this]()->bool { return idle_ > 0; });
            idle_--;

            return true;
        }
        
        /* 释放信号量一个资源 */
        void post()
        {
            std::unique_lock<std::mutex> auto_lock(lock_);

            cond_.notify_one();
            if (idle_ >= max_)
            {
                return;
            }

            idle_++;
            return;
        }
        
    private:
        
        int idle_;
        int max_;
        
        std::mutex lock_;
        std::condition_variable cond_;
        
    };
    

    
};



#endif // _CC_SEMAPHORE_H_

