

#include <sys/stat.h>

#include "file/util_dir.h"
#include "search_err.h"
#include "finder.h"
#include "filter/regex_filter.h"
#include "platform/filefetch.h"
#include "platform/win/convert/character_convert.h"

StringFinder::StringFinder()
{
    type_ = PathType::kFile;    // default
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

    
    if ((info.st_mode & DT_DIR) != 0)
        type_ = kDirectory;
    else if ((info.st_mode & DT_REG) != 0)
        type_ = kFile;
    else
        return SRH_FILETYPE;
    
    path_ = path;
    return SRH_OK;
}

SearchResultList& StringFinder::SearchString(const CPTString& pattern)
{
    results_.clear();

    if (type_ == kDirectory) {
        SearchFromDirectory();
    } else
    {
        SearchFromFile(path_);
    }
    return results_;
}

int StringFinder::SearchFromDirectory()
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
        
        ret = SearchFromFile(filepath);
        if (ret != SRH_OK)
            continue;
    }

    return SRH_OK;
}
int StringFinder::SearchFromFile(const CPTString& file)
{
    bool find = false;
    for (auto& e : filters_) {
        if (e.second.get()->Check(file))
        {
            find = true;
            break;
        }
    }

    if (!find)
        return SRH_OK;

    cptprintf(CPT("file <%s> should be search! \n"), file.c_str());
    return SRH_OK;
}

bool StringFinder::RegistFilter(std::shared_ptr<InterfaceFilter> filter)
{
    filters_[filter.get()->GetType()] = filter;
    return true;
}

