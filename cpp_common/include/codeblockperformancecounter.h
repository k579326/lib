#pragma once

/*
	统计代码块执行时间，执行次数的一个类封装，使用方法如下：

	using namespace CodeBlockPerformanceCounter;

	BlockMonitorBegin(myblock);
	```code block```
	BlockMonitorEnd(myblock);
*/

#include <unordered_set>
#include <chrono>

using namespace std::chrono;

namespace CodeBlockPerformanceCounter
{
	class BlockInfo
	{
	public:
		uint64_t invoke_times = 0;
		uint64_t tot_time = 0;			// 程序运行期间，目标代码块的总耗时	(us）
		uint64_t avg_time = 0;			// 目标代码块的平均耗时				(us)
		uint64_t max_time = 0;			// 用时最多的一次记录				(us)
		uint64_t min_time = -1;			// 用时最少的一次记录				(us)
		std::string blocktag = "";		// block tag
		std::string filename = "";		// 此block所属的文件名
		std::string funcname = "";		// 此block所属的函数名
		uint32_t linenumber = 0;		// block在文件中的行号
	};
	
	static auto HashOfBlockInfo = [](const BlockInfo& info)->size_t
	{
		std::hash<std::string> hash_string;
		std::hash<int> hashint;

		return hash_string(info.filename) ^ hash_string(info.funcname) ^
			hash_string(info.blocktag) ^ hashint(info.linenumber);
	};
	
	static auto BlockInfoEqual = [](const BlockInfo& i1, const BlockInfo& i2)->bool
	{
		return HashOfBlockInfo(i1) == HashOfBlockInfo(i2);
	};
	
	class _iN_cOunteR_CLasS
	{
	public:	
		static _iN_cOunteR_CLasS* GetInstance()
		{
			static _iN_cOunteR_CLasS _instance;
			return &_instance;
		}
		
		void RecordScope(const std::string& filename, const std::string& funcname, uint32_t linenum, 
			const std::string tag, const microseconds& costtime)
		{
			BlockInfo localtmp;
			localtmp.funcname = funcname;
			localtmp.filename = filename;
			localtmp.linenumber = linenum;
			localtmp.blocktag = tag;
			localtmp.invoke_times = 1;

			uint64_t time_interger = costtime.count();

			auto rst_pair = block_list_.insert(localtmp);
			if (!rst_pair.second)
			{
				AdjustBlockInfo(const_cast<BlockInfo*>(&(*rst_pair.first)), time_interger);
			}
			else
			{
				BlockInfo* info = const_cast<BlockInfo*>(&(*rst_pair.first));
				info->avg_time = time_interger;
				info->max_time = time_interger;
				info->min_time = time_interger;
				info->tot_time = time_interger;
			}
		}

	private:
		void Print()
		{
			printf("total record:\n");
			for (auto e : block_list_)
			{
				printf("----------\n");
				printf("%*s%-15s", 2, "", "tag:");
				printf("%s\n", e.blocktag.c_str());
				printf("%*s%-15s", 2, "", "file:");
				printf("%s\n", e.filename.c_str());
				printf("%*s%-15s", 2, "", "function:");
				printf("%s\n", e.funcname.c_str());
				printf("%*s%-15s", 2, "", "times:");
				printf("%llu\n", e.invoke_times);
				printf("%*s%-15s", 2, "", "total time:");
				printf("%llu ms\n", e.tot_time / 1000);
				printf("%*s%-15s", 2, "", "avg time:");
				printf("%llu us\n", e.avg_time);
				printf("%*s%-15s", 2, "", "max time:");
				printf("%llu us\n", e.max_time);
				printf("%*s%-15s", 2, "", "min time:");
				printf("%llu us\n", e.min_time);
				printf("\n");
			}
		}

		void AdjustBlockInfo(BlockInfo* info, uint64_t newtime)
		{
			if (newtime > info->max_time)
				info->max_time = newtime;
			if (newtime < info->min_time)
				info->min_time = newtime;

			info->invoke_times++;
			info->tot_time += newtime;
			info->avg_time = info->tot_time / info->invoke_times;
		}

	private:
		_iN_cOunteR_CLasS() : block_list_(20, HashOfBlockInfo, BlockInfoEqual)
		{
		}
		~_iN_cOunteR_CLasS() 
		{
			Print();
		}
		
		std::unordered_set<BlockInfo, decltype(HashOfBlockInfo), decltype(BlockInfoEqual)> block_list_;
	};
	
	
};



#ifndef _NDEBUG

#define BlockMonitorBegin(tag)			\
auto __bloCkMoNiTOrstart_##tag = std::chrono::steady_clock::now().time_since_epoch()


#define BlockMonitorEnd(tag)		\
auto __iNtErvAl_##tag = std::chrono::steady_clock::now().time_since_epoch() - \
__bloCkMoNiTOrstart_##tag; \
_iN_cOunteR_CLasS::GetInstance()->RecordScope(__FILE__, __FUNCTION__, __LINE__, #tag, \
duration_cast<microseconds>(__iNtErvAl_##tag))

#else
	
#define BlockMonitorBegin(tag)
#define BlockMonitorEnd(tag)

#endif





