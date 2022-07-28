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
#include <mutex>
#include <thread>
#include <map>
#include <vector>

#include <assert.h>

using namespace std::chrono;

namespace CodeBlockPerformanceCounter
{
    class BlockInfo
    {
    public:
        uint64_t start_timepoint = 0;   // ...
        uint64_t invoke_times = 0;
        uint64_t tot_time = 0;			// 程序运行期间，目标代码块的总耗时     (us)
        uint64_t avg_time = 0;			// 目标代码块的平均耗时                 (us)
        uint64_t max_time = 0;			// 用时最多的一次记录                   (us)
        uint64_t min_time = -1;			// 用时最少的一次记录				    (us)
        std::string blocktag = "";		// block tag
        std::string filename = "";		// 此block所属的文件名
        std::string funcname = "";		// 此block所属的函数名
        uint32_t linenumber = 0;		// block在文件中的行号
        uint8_t     level = 0;          // 层级关系

        bool operator==(const BlockInfo& info) const
        {
            return info.filename == this->filename && info.funcname == this->funcname
                && info.linenumber == this->linenumber;
        }
    };


    class _iN_cOunteR_CLasS;

    class coUntErMGr
    {
    private:
        coUntErMGr() {}
        ~coUntErMGr() {}

        _iN_cOunteR_CLasS* GetCurrentCounter() {
            _iN_cOunteR_CLasS* ret = nullptr;
            lock_.lock();
            auto it = instance_list_.find(std::this_thread::get_id());
            if (it != instance_list_.end())
                ret = it->second.get();
            else
            {
                auto rst = instance_list_.insert(std::make_pair(std::this_thread::get_id(), std::make_shared<_iN_cOunteR_CLasS>()));
                ret = (_iN_cOunteR_CLasS*)(rst.first->second.get());
            }
            lock_.unlock();
            return ret;
        }

    public:
        static _iN_cOunteR_CLasS* GetInstance() {
            static coUntErMGr mgr;
            return mgr.GetCurrentCounter();
        }
    private:
        std::map<std::thread::id, std::shared_ptr<_iN_cOunteR_CLasS>> instance_list_;
        std::mutex lock_;
    };


    struct Hasher
    {
        size_t operator()(const BlockInfo& info) const
        {
            std::hash<std::string> hash_string;
            std::hash<int> hashint;

            return hash_string(info.filename) ^ hash_string(info.funcname) ^
                hash_string(info.blocktag) ^ hashint(info.linenumber);
        }
    };
    struct Cmper
    {
        bool operator()(const BlockInfo& info1, const BlockInfo& info2) const
        {
            return info1 == info2;
        }
    };

    class _iN_cOunteR_CLasS
    {
    public:	
        _iN_cOunteR_CLasS() : level_(0)
        {
        }
        ~_iN_cOunteR_CLasS()
        {
            Print();
        }
        
        void RecordScopeStart(const std::string& filename, const std::string& funcname, uint32_t linenum, 
            const std::string tag, const microseconds& starttime)
        {
            BlockInfo localtmp;
            localtmp.funcname = funcname;
            localtmp.filename = filename;
            localtmp.linenumber = linenum;
            localtmp.blocktag = tag;
            localtmp.invoke_times = 0;

            localtmp.start_timepoint = starttime.count();
            
            std::lock_guard<std::mutex> autolock(lock_);
            auto rst_pair = block_list_.insert(localtmp);
            BlockInfo* info = const_cast<BlockInfo*>(&(*rst_pair.first));
            if (!rst_pair.second)
            {
                if (info->start_timepoint != 0) {
                    assert(false);
                    printf("%s(%s): BlockMonitorEnd is missing? BlockMonitorStart need match a BlockMonitorEnd!\n", 
                        __FILE__, __FUNCTION__);
                    return;
                }
                info->start_timepoint = localtmp.start_timepoint;
            }
            else
            {
                // first insert success
                info->level = level_++;
                orderlist_.push_back(info);
            }
        }
        void RecordScopeEnd(const std::string& filename, const std::string& funcname, uint32_t linenum,
            const std::string tag, const microseconds& endtime)
        {
            BlockInfo localtmp;
            localtmp.funcname = funcname;
            localtmp.filename = filename;
            localtmp.linenumber = linenum;
            localtmp.blocktag = tag;

            uint64_t interval = endtime.count();

            std::lock_guard<std::mutex> autolock(lock_);
            auto iterator = block_list_.find(localtmp);

            //auto rst_pair = block_list_.insert(localtmp);
            if (iterator != block_list_.end() && iterator->start_timepoint != 0)
            {
                BlockInfo* info = const_cast<BlockInfo*>(&(*iterator));
                interval = interval - info->start_timepoint;
                AdjustBlockInfo(info, interval);
                info->start_timepoint = 0;
                level_--;
            }
            else /* !rst_pair.second || rst_pair.first->start_timepoint == 0 */
            {
                // miss BlockMonitorBegin label?
                assert(false);
            }
            return;
        }
    private:
        void Print()
        {
            printf("total record:\n");
            for (auto e : orderlist_)
            {
                int intent = 4 * (e->level + 1);
                printf("%d%*s----------\n", e->level, intent, "");
                printf("%*s+ %-15s", intent, "", "tag:");
                printf("%s\n", e->blocktag.c_str());
                printf("%*s+ %-15s", intent, "", "file:");
                printf("%s\n", e->filename.c_str());
                printf("%*s+ %-15s", intent, "", "function:");
                printf("%s\n", e->funcname.c_str());
                printf("%*s+ %-15s", intent, "", "times:");
                printf("%llu\n", e->invoke_times);
                printf("%*s+ %-15s", intent, "", "total time:");
                printf("%llu ms\n", e->tot_time / 1000);
                printf("%*s+ %-15s", intent, "", "avg time:");
                printf("%llu us\n", e->avg_time);
                printf("%*s+ %-15s", intent, "", "max time:");
                printf("%llu us\n", e->max_time);
                printf("%*s+ %-15s", intent, "", "min time:");
                printf("%llu us\n", e->min_time);
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
        std::unordered_set<BlockInfo, Hasher, Cmper> block_list_;
        std::vector<BlockInfo*> orderlist_;
        std::mutex lock_;
        uint8_t level_;
    };

};


#ifndef COUNTER_ENABLE
#define COUNTER_ENABLE
#endif


#if defined COUNTER_ENABLE && (!defined NDEBUG && !defined _NDEBUG)

#define BlockMonitorBegin(tag)			\
uint32_t lInENumBEr_##tag = __LINE__;	\
auto __bloCkMoNiTOrstart_##tag = std::chrono::steady_clock::now().time_since_epoch(); \
coUntErMGr::GetInstance()->RecordScopeStart(__FILE__, __FUNCTION__, lInENumBEr_##tag, #tag, \
duration_cast<microseconds>(__bloCkMoNiTOrstart_##tag))


#define BlockMonitorEnd(tag)		\
auto __bloCkMoNiTOreNd_##tag = std::chrono::steady_clock::now().time_since_epoch();\
coUntErMGr::GetInstance()->RecordScopeEnd(__FILE__, __FUNCTION__, lInENumBEr_##tag, #tag, \
duration_cast<microseconds>(__bloCkMoNiTOreNd_##tag))

#else
    
#define BlockMonitorBegin(tag)
#define BlockMonitorEnd(tag)

#endif





