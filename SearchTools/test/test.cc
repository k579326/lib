

#include "finder.h"
#include "regex_filter.h"
#include "logger.h"

void callback(SearchResultList* rts) {
    LOG_Debug("callback trigged, result count = %d", (*rts).size());
    for (int i = 0; i < (*rts).size(); i++)
    {
        LOG_Infor("%s, <%s>, %d",
            (*rts)[i].first.filepath.c_str(),
            (*rts)[i].first.encode_string.c_str(),
            (*rts)[i].second.size());
    }
    rts->clear();
    rts->reserve(0);
}

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        return -1;
    }

    //std::string testdir("/home/K/Desktop/SearchTools");
    std::string testdir(argv[1]);

    std::shared_ptr<RegexFilter<CPTString>> filter = std::make_shared<RegexFilter<CPTString>>(false);
    filter->SetFilters({argv[3]}, true);

    SearchSetting setting{false, true};
    StringFinder finder(setting);
    finder.RegistFilter(filter);

    finder.SetSearchPath(testdir);
    finder.SetPattern<ANSI>(argv[2], false);

    LOG_Warning("Begin search \"%s\" from %s...", argv[2], argv[1]);
    finder.DoSearch(callback);

    getchar();

    return 0;
}
