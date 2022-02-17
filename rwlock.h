#pragma once

#include <assert.h>

#include <mutex>
#include <condition_variable>

class RwLock
{
public:
	RwLock() {}
	~RwLock() {}
	
	void RLock()
	{
		std::unique_lock<std::mutex> autolock(mtx_);
		if (wr_flag_)
		{
			cond_.wait(autolock, [this]()->bool { return !wr_flag_; });
			bool x = wr_flag_;
		}
		rd_count_++;
		// printf("read lock \n");
		return;
	}
	
	void WLock()
	{
		std::unique_lock<std::mutex> autolock(mtx_);
		wr_flag_ = true;

		cond_.wait(autolock, [this]()->bool { return rd_count_ == 0 && 0 == wr_count_; });
		wr_flag_ = true;

		wr_count_++;
		// printf("write lock \n");
		return;
	}
	
	void RUnlock()
	{
		std::unique_lock<std::mutex> autolock(mtx_);
		if (wr_count_)
		{
			assert(false);
			return;
		}
		if (rd_count_)
		{
			rd_count_--;
			cond_.notify_all();
			//printf("read unlock \n");
		}
		else assert(false);
	}
	void WUnlock()
	{
		std::unique_lock<std::mutex> autolock(mtx_);
		if (wr_count_)
		{
			wr_flag_ = false;
			wr_count_--;
			cond_.notify_all();
			//printf("write unlock \n");
		}
		else assert(false);
	}
	
private:
	std::mutex mtx_;
	std::condition_variable cond_;
	size_t rd_count_ = 0;
	size_t wr_count_ = 0;
	bool wr_flag_ = false;
};