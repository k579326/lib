#ifndef _UTIL_THREADPOOL_H_
#define _UTIL_THREADPOOL_H_


#include <assert.h>

#include <thread>
#include <atomic>
#include <memory>
#include <queue>
#include <vector>
#include <list>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace threads
{

template<class T>
using ThreadProc = std::function<void(std::shared_ptr<T>)>;

template<class T>
using FinCallback = std::function<void(std::shared_ptr<T>)>;

using _ThreadProc = std::function<void(void)>;
using _FinCallBack = std::function<void(void)>;


class ThreadPool
{
private:
    class Thread
    {
    public:
        Thread(std::thread&& thread)
        {
            thread_ = std::move(thread);
            is_working_ = false;
            is_del_ = false;
        }
        Thread() {
            is_working_ = false;
            is_del_ = false;
        }

        ~Thread()
        {
            if (thread_.joinable())
                thread_.join();
            is_working_ = false;
            is_del_ = true;
        }
        Thread(const Thread& other) noexcept {
            is_working_ = false;
            is_del_ = false;
        }
        Thread(Thread&& other) noexcept {
            thread_ = std::move(other.thread_);
            is_working_ = false;
            is_del_ = false;
        }

        std::thread thread_;
        bool        is_working_;        // �Ƿ�����ִ������
        bool        is_del_;            // ��ɾ����־�����ڶ�̬�����߳���
    };

protected:
    ThreadPool();
public:
    ~ThreadPool();
    
    static ThreadPool* CreateInstance(size_t size);
    static ThreadPool* GetInstance();

    int init(size_t size);

    template<class T>
    void PushWorkQueue(ThreadProc<T> proc, std::shared_ptr<T> param, FinCallback<T> fin);

    void PushWorkQueue(std::function<void(void)> proc);

    // ����̣߳������ӵ��߳������ڿ����߳������򷵻�false��ִ��ʧ��
    bool AddThreads(size_t count);

    // �����̣߳����ٵ��߳���������ڿ����߳������򷵻�false��ִ��ʧ�ܡ�
    bool DelThreads(size_t count);

    void CancelAll();

private:
    bool _IsQuit();
    void _ThreadProcess(std::shared_ptr<Thread> thread_ctx);

private:    
    const int max_size_ = 16;                    // �߳�����չ����
    
    std::queue<std::pair<_ThreadProc, _FinCallBack> > queue_;
    std::list<std::shared_ptr<Thread>>    pool_;

    size_t idle_count_ = 0;                          // ����״̬���߳���

    std::atomic<bool>           exit_;
    std::mutex                  mutex_;
    std::condition_variable     cond_;

    static ThreadPool* ptr_;
};


template<class T>
inline void ThreadPool::PushWorkQueue(ThreadProc<T> proc, std::shared_ptr<T> param, FinCallback<T> fin)
{
    _ThreadProc tf = std::bind(proc, param);
    _FinCallBack ff = []()->void {}; 

    if (fin)
    {
        ff = std::bind(fin, param);
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (exit_)
        return;
    queue_.push(std::make_pair(tf, ff));
    cond_.notify_one();
}

inline void ThreadPool::PushWorkQueue(std::function<void(void)> proc)
{
    assert(proc);
    
    _FinCallBack ff = []()->void {}; 

    std::lock_guard<std::mutex> lock(mutex_);
    if (exit_)
        return;
    queue_.push(std::make_pair(proc, ff));
    cond_.notify_one();
}


};





#endif // _UTIL_THREADPOOL_H_


