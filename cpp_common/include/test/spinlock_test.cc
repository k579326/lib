

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <codeblockperformancecounter.h>
using namespace CodeBlockPerformanceCounter;

#define USE_SPINLOCK 1
//#define USE_BOOST_SPINLOCK

#if USE_SPINLOCK
#ifndef USE_BOOST_SPINLOCK
#include "spinlock.h"
#else
#include "spinlock.hpp"
using SpinLock = boost::sort::common::spinlock_t;
#endif
#else

using SpinLock = std::mutex;

#endif

SpinLock lock;
int test_number = 0;

void thread1_func()
{
    int loop_time = 100000;
    BlockMonitorBegin(gg);
    while (loop_time--)
    {
        lock.lock();
        test_number++;
        lock.unlock();
        // printf("%d\n", test_number);
    }
    BlockMonitorEnd(gg);
    printf("sub thread stop!\n");
}


int main()
{
    std::thread t1(thread1_func);
    //std::thread t2(thread1_func);
    //std::thread t3(thread1_func);
    //std::thread t4(thread1_func);
    // std::this_thread::sleep_for(std::chrono::seconds(200));
    if (t1.joinable())
        t1.join();
    //if (t2.joinable())
    //    t2.join();
    //if (t3.joinable())
    //    t3.join();
    //if (t4.joinable())
    //    t4.join();
    return 0;
}
