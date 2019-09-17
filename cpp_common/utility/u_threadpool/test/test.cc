

#include "util_threadpool.h"




class threadfunc
{
public:
    void func(std::shared_ptr<float> a)
    {
        printf("%s\n", __FUNCTION__);
    }
};




void func(std::shared_ptr<float> a)
{
    printf("%s\n", __FUNCTION__);
}

void func1(std::shared_ptr<int> a)
{
    printf("%s\n", __FUNCTION__);
}

int main()
{
    util_threadpool::ThreadPool pool(2);
    threadfunc f;

    pool.PushWorkQueue<float>(std::bind(&threadfunc::func, &f, std::placeholders::_1), std::make_shared<float>(1.0f), &func);
    pool.PushWorkQueue<float>(&func, std::make_shared<float>(1.0f), &func);
    pool.PushWorkQueue<int>(&func1, std::make_shared<int>(1), nullptr);
    pool.PushWorkQueue<float>(&func, nullptr, nullptr);
    pool.PushWorkQueue<float>(&func);


    // 
    // std::this_thread::sleep_for(std::chrono::seconds(20));
    // 
    // pool.PushWorkQueue<float>(std::bind(&threadfunc::func, &f, std::placeholders::_1), std::make_shared<float>(1.0), &func);
    // pool.PushWorkQueue<float>(&func, std::make_shared<float>(1.0), &func);
    // pool.PushWorkQueue<int>(&func1);
    // pool.PushWorkQueue(std::make_shared<int>(1));
    // pool.PushWorkQueue<float>(&func, nullptr, nullptr);

    std::this_thread::sleep_for(std::chrono::seconds(20));


    std::this_thread::sleep_for(std::chrono::seconds(2000));
    return 0;
}

