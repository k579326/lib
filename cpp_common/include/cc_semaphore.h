

#ifndef _PWRD_SEMAPHORE_H_
#define _PWRD_SEMAPHORE_H_

#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>


namespace pwrd_semaphore
{
    class semaphore
    {
    public:
        semaphore(int size);
        ~semaphore();
        
        semaphore(const semaphore&) = delete;
        semaphore(semaphore&&) = delete;
        semaphore& operator=(const semaphore&) = delete;
        semaphore& operator==(semaphore&&) = delete;
        
        /* 占用一个资源，最多等待ms毫秒, 如果为-1则不返回 */
        int wait(std::chrono::milliseconds ms);
        
        /* 获取到了信号量则返回true, 否则返回false */
        bool try_wait();
        
        /* 释放信号量一个资源 */
        void post();
        
        
    private:
        
        int cur_;
        int max_;
    };
    
    
};











#endif // _PWRD_SEMAPHORE_H_

