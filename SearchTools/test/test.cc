

#include "finder.h"
#include "regex_filter.h"


int main()
{
    //std::string testdir("/home/K/Desktop/SearchTools");
    std::wstring testdir(L"F:\\ѧϰ\\c++ test\\SearchString");

    std::shared_ptr<RegexFilter<CPTString>> filter = std::make_shared<RegexFilter<CPTString>>(true, false);
    filter->SetFilters({L"*.h", L"*.exe", L"CMake*"});

    StringFinder finder;
    finder.RegistFilter(filter);

    finder.SetSearchPath(testdir);
    finder.SearchString(L"");

    return 0;

}
