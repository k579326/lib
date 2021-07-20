#ifndef _FINDER_H_
#define _FINDER_H_

#include <list>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>

#include "u_encode/encode_def.h"
#include "search_def.h"
#include "KMP_EX.h"
#include "filter.h"
#include "platform/filemapping.h"


class FileInfo
{
public:
    CPTString   filepath;
    size_t      filesize;
    CPTString   encode_string;
};

struct SearchSetting
{
    bool icase; // no use
    bool recursive;
};


using OffsetSet = std::set<size_t>;
using SearchResult = std::pair<FileInfo, OffsetSet>;
using SearchResultList = std::vector<SearchResult>;
using SearchCallBack = std::function<void(SearchResultList* results)>;

namespace threads { class ThreadPool; };

class StringFinder
{
public:
    enum class PathType {
        kDirectory,
        kFile
    };
    StringFinder(const SearchSetting& );
    ~StringFinder() {}

    void Init(const SearchSetting& );
    // path can be file or directory
    int SetSearchPath(const CPTString& path);
    

    template<EncodeType _ET, class _Traits = Encodetraits<_ET>>
    void SetPattern(const typename _Traits::Elm* pattern, bool icase)
    {
        ptn_mgr_.get()->InputPattern<_ET>(pattern, typename _Traits::GetLength(pattern), icase);
        /*
        if (_ET == EncodeType::ANSI)
            ptn_mgr_.get()->InputPatternAnsi(pattern, typename _Traits::GetLength(pattern));
        else if (_ET == EncodeType::UTF8 || _ET == EncodeType::UTF8_BOM)
            ptn_mgr_.get()->InputPatternUtf8(pattern, typename _Traits::GetLength(pattern));
        else if (_ET == EncodeType::UTF16_LE)
            ptn_mgr_.get()->InputPatternUtf16LE(pattern, typename _Traits::GetLength(pattern));
        else if (_ET == EncodeType::UTF16_BE)
            ptn_mgr_.get()->InputPatternUtf16BE(pattern, typename _Traits::GetLength(pattern));
        */
        return;
    }

    void DoSearch(SearchCallBack cb);
    bool RegistFilter(std::shared_ptr<InterfaceFilter> filter);
    bool GetResult(SearchResultList& result);

private:
    int ListFromDirectory();
    void Work(const CPTString& file);

    template<EncodeType _ET, class _Traits = Encodetraits<_ET>>
    void MakeResult(const FileMapping::MapInfo& info)
    {
        KMPExtention<_ET> kmp(setting_.icase);
        std::shared_ptr<Pattern<_ET>> ansiptn =
            std::dynamic_pointer_cast<Pattern<_ET>>(ptn_mgr_->GetPattern(_ET));
        kmp.SetPattern(ansiptn);
        kmp.SetSource((_Traits::Elm*)info.addr, info.len / _Traits::_ElementSize);
        auto match_rst = kmp.getAllMatch();
        if (match_rst.empty())
            return;

        OffsetSet sets;
        FileInfo finfo;
        for (auto& e : match_rst) {
            sets.insert((uint8_t*)e - (uint8_t*)info.addr);
        }
        finfo.filepath = info.filepath;
        finfo.encode_string = __ET_string[_ET];

        mtx_.lock();
        results_.push_back(std::make_pair(finfo, sets));
        mtx_.unlock();
    }

private:
    std::map<EncodeType, std::string> __ET_string =
    {
        {UTF8, "UTF-8"}, {ANSI, "ANSI"}, {UTF16_LE, "UTF-16 LE"}, 
        {UTF16_BE, "UTF-16 BE"}, {UTF8_BOM, "UTF-8 BOM"}
    };

private:

    SearchSetting setting_;
    std::atomic_uint32_t workers_ = 0;
    std::map<FilterType, std::shared_ptr<InterfaceFilter>> filters_; 
    SearchResultList results_;
    CPTString path_;   // directory or file
    PathType type_;
    std::list<CPTString> files_;
    std::unique_ptr<PatternMgr> ptn_mgr_;
    SearchCallBack finish_cb_;

    threads::ThreadPool* trdpool;
    std::condition_variable cond_;
    std::mutex mtx_;
};









#endif // _FINDER_H_
