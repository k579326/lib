#ifndef _FINDER_H_
#define _FINDER_H_

#include <vector>
#include <string>
#include <set>
#include <map>
#include <memory>

#include "search_def.h"
#include "filter.h"


using OffsetSet = std::set<size_t>;
using SearchResult = std::pair<CPTString, OffsetSet>;
using SearchResultList = std::vector<SearchResult>;



class StringFinder
{
    enum PathType {
        kDirectory,
        kFile
    };
public:
    StringFinder();
    ~StringFinder() {}

    // path can be file or directory
    int SetSearchPath(const CPTString& path);
    SearchResultList& SearchString(const CPTString& pattern);
    bool RegistFilter(std::shared_ptr<InterfaceFilter> filter);

private:
    int SearchFromDirectory();
    int SearchFromFile(const CPTString& file);
private:

    std::map<FilterType, std::shared_ptr<InterfaceFilter>> filters_; 
    SearchResultList results_;
    CPTString path_;   // directory or file
    PathType type_;

};









#endif // _FINDER_H_
