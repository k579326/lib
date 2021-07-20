
#include "filefetch.h"

#include <string.h>

#include "search_def.h"
#include "u_dir/util_dir.h"
#include "search_err.h"


FetchFile::FetchFile(const CPTString& directory)
{
    rootdir_ = directory;
    curdir_ = directory;
    childdirs_.push(rootdir_);
}

FetchFile::~FetchFile()
{
    if (dstate_)
        closedir(dstate_);
}

bool FetchFile::OpenDir()
{
    curdir_ = childdirs_.front();
#if defined(UNICODE) || defined(_UNICODE)
    char* mbstr = WideStringToAnsi(curdir_.c_str());
    dstate_ = opendir(mbstr);
    free(mbstr);
#else 
    dstate_ = opendir(curdir_.c_str());
#endif 
    if (!dstate_)
        return false;

    return true;
}

int FetchFile::Fetch(CPTString& filepath)
{   
    int err = SRH_NOEXIST;
    while (!childdirs_.empty())
    {        
        err = GetFileFullPathFromDir(filepath);
        if (err == SRH_NOEXIST)
            childdirs_.pop();
        else 
            break;
    }

    return err;
}

int FetchFile::GetFileFullPathFromDir(CPTString& filepath)
{
    if (!dstate_ && !OpenDir())
        return SRH_OTHER;

    struct dirent* info = readdir(dstate_);

    while (info)
    {
        if (strcmp(info->d_name, ".") == 0 ||
            strcmp(info->d_name, "..") == 0)
        {
            info = readdir(dstate_);
            continue;
        }

        if (!info) 
            break;
        
        CPTString tmppath;
        CPTString filename;
#if defined(UNICODE) || defined(_UNICODE)
        wchar_t* str = AnsiToWideString(info->d_name);
        filename = str;
        free(str);
#else
        filename = info->d_name;
#endif
        tmppath = curdir_ + CPT('/') + filename;
        if (S_ISDIR(info->d_type))
        {
            //printf("fetch find directory: %s", tmppath.c_str());
            childdirs_.push(tmppath);
        }
        else {
            //printf("fetch find file: %s", tmppath.c_str());
            filepath = tmppath;
            break;
        }
        info = readdir(dstate_);
    } 

    if (!info)
    {
        closedir(dstate_);
        dstate_ = nullptr;
        return SRH_NOEXIST;
    }

    return SRH_OK;
}
