
#include "../cc_semaphore.h"


/*

1、开启 200 个线程，初始化一个最大值为3的信号量；
2、

*/

#include <thread>
#include <array>
#include <iostream>
#include <functional>
#include <vector>

int test_wait()
{
    std::array<std::thread, 200> threadlist;

    cc_semaphore::semaphore sem(3);
    std::vector<int> recordlist;

    auto thread_func = [&sem](std::vector<int>& recordlist, int thread_num)->void {
    
        while (1)
        {
            if (sem.wait(std::chrono::milliseconds(-1)))
            {
                printf("post time: %llu\n", std::chrono::high_resolution_clock::now());
                printf("thread %d : get semaphore!\n", thread_num);
                // std::this_thread::sleep_for(std::chrono::seconds(10));
                // sem.post();
                // printf("thread %d : release semaphore!\n", thread_num);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    };

    int thread_num = 0;
    std::for_each(threadlist.begin(), threadlist.end(), 
                  [thread_func, &thread_num, &recordlist](std::thread& const t)->void 
                    { 
                        t.swap(std::thread(std::bind(thread_func, std::placeholders::_1, thread_num++), recordlist)); 
                        t.detach();
                    });


    while (1)
    {
        getchar();
        sem.post();
        printf("post time: %llu\n", std::chrono::high_resolution_clock::now());
    }



    std::this_thread::sleep_for(std::chrono::hours(1));

    return 0;
}


int test_waitfor()
{
    std::array<std::thread, 200> threadlist;

    cc_semaphore::semaphore sem(3);
    std::vector<int> recordlist;

    auto thread_func = [&sem](std::vector<int>& recordlist, int thread_num)->void {

        while (1)
        {
            if (sem.try_wait())
            {
                printf("thread %d : get semaphore!\n", thread_num);
                // std::this_thread::sleep_for(std::chrono::seconds(10));
                // sem.post();
                // printf("thread %d : release semaphore!\n", thread_num);
            }
            else
            {
            }
        }
    };

    int thread_num = 0;
    std::for_each(threadlist.begin(), threadlist.end(),
                  [thread_func, &thread_num, &recordlist](std::thread& const t)->void
    {
        t.swap(std::thread(std::bind(thread_func, std::placeholders::_1, thread_num++), recordlist));
        t.detach();
    });


    while (1)
    {
        getchar();
        sem.post();
    }



    std::this_thread::sleep_for(std::chrono::hours(1));

    return 0;
}


int main()
{

    test_wait();

    return 0;
}


