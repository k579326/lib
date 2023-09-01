

#include <codeblockperformancecounter.h>
using namespace CodeBlockPerformanceCounter;

#define USE_SPINLOCK 1
#define USE_BOOST_SPINLOCK 0

#if USE_SPINLOCK
#if !USE_BOOST_SPINLOCK
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
int stop = 100000;

void thread1_func()
{
    BlockMonitorBegin(gg);
    while (stop)
    {
        stop--;
        lock.lock();
        test_number++;
        lock.unlock();
        // printf("%d\n", test_number);
    }
    BlockMonitorEnd(gg);
    printf("sub thread stop!\n");
}


void thread2_func()
{
    while (stop)
    {
        lock.lock();
        test_number = 0;
        lock.unlock();
    }
}

int main()
{
    std::thread t1(thread1_func);
    std::thread t2(thread2_func);
    // std::this_thread::sleep_for(std::chrono::seconds(200));
    if (t1.joinable())
        t1.join();
    if (t2.joinable())
        t2.join();
    return 0;
}
