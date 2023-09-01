#pragma once

#include <atomic>
#include <thread>

class SpinLock
{
public:
    SpinLock() { is_locked_.store(false); }
    ~SpinLock() {}
    
    SpinLock(const SpinLock&) = delete;
    SpinLock(SpinLock&&) = delete;
    SpinLock& operator=(const SpinLock&) = delete;
    SpinLock& operator=(SpinLock&&) = delete;
    
    void lock() 
    {
        // 这里没必要使用compare_exchange_weak, compare_exchange_weak比exchange复杂，耗时更多
        while (is_locked_.exchange(true, std::memory_order_acquire))
        {
            std::this_thread::yield();
        }
    }
    void unlock() 
    {
        // 解锁不需要block thread
        is_locked_.store(false, std::memory_order_release);
    }

private:
    std::atomic<bool> is_locked_;
};


class SpinLockGuard
{
public:
    SpinLockGuard(SpinLock& lock) : lock_(lock) {
        lock_.lock();
    }
    ~SpinLockGuard() {
        lock_.unlock();
    }
    
    SpinLockGuard(const SpinLockGuard&) = delete;
    SpinLockGuard(SpinLockGuard&&) = delete;
    SpinLockGuard& operator=(const SpinLockGuard&) = delete;
    SpinLockGuard& operator=(SpinLockGuard&&) = delete;

private:
    SpinLock& lock_;
};










