

#pragma once


class ILock
{
public:
	ILock(){}
	virtual ~ILock() {}
	
	virtual void lock() = 0;
	virtual void unlock() = 0;
private:
};


#ifdef _WIN32
#include <windows.h>

class xLock : public ILock
{
public:
	xLock()
	{
		InitializeCriticalSection(&cs);
	}
	
	virtual ~xLock()
	{
		DeleteCriticalSection(&cs);
	}
	
	void lock() override
	{
		EnterCriticalSection(&cs);
	}
	
	void unlock() override
	{
		LeaveCriticalSection(&cs);
	}
	
private:
	CRITICAL_SECTION cs;
};

#elif defined __unix__
#include <pthread.h>

class xLock : public ILock
{
public:
	xLock()
	{
		pthread_mutex_init(&mutex, NULL);
	}
	
	virtual ~xLock()
	{
		pthread_mutex_destroy(&mutex);
	}
	
	void lock() override
	{
		pthread_mutex_lock(&mutex);
	}
	
	void unlock() override
	{
		pthread_mutex_unlock(&mutex);
	}
	
private:
	pthread_mutex_t mutex;
};

#endif


class AutoLock
{
public:
	AutoLock(ILock* lock) : lockInstance(lock)
	{
		lockInstance->lock();
	}
	
	virtual ~AutoLock()
	{
		lockInstance->unlock();
	}
private:
	
	ILock* lockInstance;
};

