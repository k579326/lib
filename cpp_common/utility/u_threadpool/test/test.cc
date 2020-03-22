

#include "util_threadpool.h"




class threadfunc
{
public:
    void func(std::shared_ptr<float> a)
    {
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            //printf("%s\n", __FUNCTION__);
            break;
        }
    }
};

void func(std::shared_ptr<float> a)
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(11));
        //printf("%s\n", __FUNCTION__);
        break;
    }
}

void func1(std::shared_ptr<int> a)
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(13));
        //printf("%s\n", __FUNCTION__);
        break;
    }
}

int main()
{
    threads::ThreadPool* pool = threads::ThreadPool::CreateInstance(20);
    threadfunc f;

    auto task_add = [&]()->void {
        for (int i = 0; i < 30000; i++)
        {
            pool->PushWorkQueue<float>(std::bind(&threadfunc::func, &f, std::placeholders::_1), std::make_shared<float>(1.0f), &func);
            std::this_thread::sleep_for(std::chrono::milliseconds(12));
            pool->PushWorkQueue<float>(&func, std::make_shared<float>(1.0f), &func);
        }
    };

    std::thread add_thread(task_add);
    // pool->PushWorkQueue<float>(std::bind(&threadfunc::func, &f, std::placeholders::_1), std::make_shared<float>(1.0f), &func);
    // pool->PushWorkQueue<float>(&func, std::make_shared<float>(1.0f), &func);
    // pool->PushWorkQueue<int>(&func1, std::make_shared<int>(1), nullptr);
    // pool->PushWorkQueue<float>(&func, nullptr, nullptr);

    auto change_operation = [pool]()->void {
        bool ret;
        for (int i = 0; i < 1000; i++)
        {
            size_t add_count, del_count;
            add_count = rand() % 16;
            del_count = rand() % 16;
            ret = pool->DelThreads(del_count);
            printf("Delete %llu thread %d\n", del_count, (int)ret);
            std::this_thread::sleep_for(std::chrono::milliseconds(23));
            ret = pool->AddThreads(add_count);
            printf("Delete %llu thread %d\n", del_count, (int)ret);
            std::this_thread::sleep_for(std::chrono::milliseconds(17));
        }

    };

    std::thread change_thread(change_operation);

    // pool->PushWorkQueue<float>(&func);


    // 
    // std::this_thread::sleep_for(std::chrono::seconds(20));
    // 
    // pool.PushWorkQueue<float>(std::bind(&threadfunc::func, &f, std::placeholders::_1), std::make_shared<float>(1.0), &func);
    // pool.PushWorkQueue<float>(&func, std::make_shared<float>(1.0), &func);
    // pool.PushWorkQueue<int>(&func1);
    // pool.PushWorkQueue(std::make_shared<int>(1));
    // pool.PushWorkQueue<float>(&func, nullptr, nullptr);

    std::this_thread::sleep_for(std::chrono::seconds(18));
    pool->CancelAll();

    std::this_thread::sleep_for(std::chrono::seconds(2000));
    return 0;
}

