#ifndef _FILE_FETCH_H_
#define _FILE_FETCH_H_

#include <memory>
#include <string>
#include <queue>

#include "u_dir/util_dir.h"


#if  !defined(_UNICODE) && !defined(UNICODE)
using CPTString = std::string;
#else
using CPTString = std::wstring;
#endif

class FetchFile
{
public:
    FetchFile(const CPTString& directory);
    ~FetchFile();

    int Fetch(CPTString& filepath);

private:
    bool OpenDir();
    int GetFileFullPathFromDir(CPTString& filepath);

private:

    DIR* dstate_ = nullptr;
    CPTString rootdir_;
    CPTString curdir_;
    std::queue<CPTString> childdirs_;
};





#endif // _FILE_FETCH_H_

