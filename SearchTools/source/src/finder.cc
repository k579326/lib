

#include "finder.h"

#include <sys/stat.h>

#include "u_threadpool/util_threadpool.h"
#include "u_dir/util_dir.h"
#include "search_err.h"
#include "filter/regex_filter.h"
#include "platform/filefetch.h"
#include "platform/filemapping.h"
#include "u_encode/check_encode.h"
#include "KMP_EX.h"

#include "logger.h"

StringFinder::StringFinder(const SearchSetting& setting)
{
    type_ = PathType::kFile;    // default
    setting_.icase = setting.icase;
    setting_.recursive = setting.recursive;
    ptn_mgr_ = std::make_unique<PatternMgr>();
    LOG_InitAsConsoleMode("stringfinder", "1.0", "./");
    trdpool = threads::ThreadPool::CreateInstance(4);
}

void StringFinder::Init(const SearchSetting& setting)
{
    setting_ = setting;
}


int StringFinder::SetSearchPath(const CPTString& path)
{
    if (path.empty()) 
        return SRH_INVAL;
    struct stat info = { 0 };

#if defined(UNICODE) || defined(_UNICODE)
    char* mbstr = WideStringToAnsi(path.c_str());
    if (0 != stat(mbstr, &info)) {
        free(mbstr);
        return SRH_NOEXIST;
    }
    free(mbstr);
#else 
    if (0 != stat(path.c_str(), &info)) {
        return SRH_NOEXIST;
    }
#endif
    
    if (S_ISDIR(info.st_mode))
        type_ = PathType::kDirectory;
    else if (S_ISREG(info.st_mode))
        type_ = PathType::kFile;
    else
        return SRH_FILETYPE;
    
    path_ = path;
    return SRH_OK;
}

void StringFinder::DoSearch(SearchCallBack cb)
{
    finish_cb_ = cb;
    if (type_ == PathType::kDirectory) {
        ListFromDirectory();
    } else
    {
        files_.push_back(path_);
    }

    workers_ = files_.size();
    for (auto e : files_) {
        auto func = std::bind(&StringFinder::Work, this, e);
        trdpool->PushWorkQueue(func);
    }
    return;
}

int StringFinder::ListFromDirectory()
{
    CPTString filepath;
    std::unique_ptr<FetchFile> fetcher_(new FetchFile(path_));
    
    while (true)
    {
        int ret = fetcher_.get()->Fetch(filepath);
        if (ret == SRH_NOEXIST)
            return SRH_OK;
        else if (ret != SRH_OK)
            continue;
        
        for (auto& e : filters_) {
            if (e.second.get()->Check(filepath))
            {
                files_.push_back(filepath);
                break;
            }
        }
    }

    return SRH_OK;
}

bool StringFinder::RegistFilter(std::shared_ptr<InterfaceFilter> filter)
{
    filters_[filter.get()->GetType()] = filter;
    return true;
}

 bool StringFinder::GetResult(SearchResultList& result)
{
     return false;
}

 void StringFinder::Work(const CPTString& file)
 {
     std::shared_ptr<FileMapping> fmap = std::make_shared<FileMapping>();
     FileMapping::MapInfo info;

     EncodeType et;
     fmap->SetFile(file);
     if (fmap->OpenMapping() != SRH_OK) {
         LOG_Error("file %s have been skipped, may be it is too large or not exist!", file.c_str());
         goto _EXIT_;
     }

     while (true) {
         if (fmap->Mapping(info) != SRH_OK)
             break;
         if (!info.addr)
             goto _EXIT_;
         else
             break;
     }
     if (!info.addr)
         goto _EXIT_;

     et = CheckEncode::Check((const uint8_t*)info.addr, info.len);
     if (et == EncodeType::BadStream)
         goto _EXIT_;
     if (et == EncodeType::ANSI) {
         MakeResult<ANSI>(info);
     }
     else if (et == EncodeType::UTF8 || et == EncodeType::UTF8_BOM) {
         if (et == EncodeType::UTF8_BOM) {
             uint8_t** p = (uint8_t**)&info.addr;
             *p = *p + 3;
         }
         MakeResult<UTF8>(info);
     }
     else if (et == EncodeType::UTF16_LE) {
         uint8_t** p = (uint8_t**)&info.addr;
         *p = *p + 2;
         MakeResult<UTF16_LE>(info);
     }
     else if (et == EncodeType::UTF16_BE) {
         uint8_t** p = (uint8_t**)&info.addr;
         *p = *p + 2;
         MakeResult<UTF16_BE>(info);
     }
     else {}

_EXIT_:
     workers_--;
     if (workers_ == 0) {
         finish_cb_(&results_);
     }

     return;
 }

