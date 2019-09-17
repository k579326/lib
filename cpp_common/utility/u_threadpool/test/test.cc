

#include "util_threadpool.h"


static void func1()
{
    printf("%s\n", __FUNCTION__);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

static void func2(void* ptr)
{
    printf("%s\n", __FUNCTION__);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

static int func3(int a, char b, void* c)
{
    printf("%s\n", __FUNCTION__);
    return 0;
}

class threadfunc
{
public:
    void func(int a)
    {
        printf("%s\n", __FUNCTION__);
    }
};



int main()
{
    util_threadpool::ThreadPool pool(2);
    threadfunc f;

    pool.PushWorkQueue(std::bind(&func1));
    pool.PushWorkQueue(std::bind(&func2, nullptr));
    pool.PushWorkQueue(std::bind(&func3, 1, (char)2, nullptr));
    pool.PushWorkQueue(std::bind(&threadfunc::func, &f, 1));

    std::this_thread::sleep_for(std::chrono::seconds(20));

    pool.PushWorkQueue(std::bind(&func1));
    pool.PushWorkQueue(std::bind(&func2, nullptr));
    pool.PushWorkQueue(std::bind(&func3, 1, (char)2, nullptr));
    pool.PushWorkQueue(std::bind(&threadfunc::func, &f, 1));

    std::this_thread::sleep_for(std::chrono::seconds(20));


    std::this_thread::sleep_for(std::chrono::seconds(2000));
    return 0;
}




