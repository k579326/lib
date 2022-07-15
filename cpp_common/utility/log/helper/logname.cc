
#include "logname.h"

#include <assert.h>

#include "file/util_file.h"
#include "string/util_string.h"

LogNameManager::LogNameManager(const CptString& path, const CptString& label) 
    : path_(path), label_(label), separator_(TEXT('#'))
{
}

LogNameManager::~LogNameManager()
{

}

void LogNameManager::SetPath(const CptString& path)
{
    path_ = path;
}
void LogNameManager::SetLabel(const CptString& label)
{
    label_ = label;
}


CptString LogNameManager::GetLogName() const
{
    return GetLogName(std::chrono::system_clock::now());
}

bool LogNameManager::NeedUpdate() const
{
    return !fileutil::File::IsExist(GetLogName());
}

CptString LogNameManager::GetLogName(const std::chrono::system_clock::time_point& timepoint) const
{
    time_t timestamp = std::chrono::system_clock::to_time_t(timepoint);

    CptChar timestr[64];
    tm* tp = localtime(&timestamp);
    CptStringPrintf(timestr, TEXT("%4d-%02d-%02d"), tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday);

    CptString filename = CptString(timestr) + separator_ + label_;
    CptString fullpath = path_ + TEXT("/") + filename + TEXT(".log");

    return fullpath;
}


std::chrono::system_clock::time_point
LogNameManager::GetDateOfLogName(const CptString& logname) const
{
    // 拆分 {date}#{gamename}.log
    std::vector<CptString> arr = stringutil::Split(logname, separator_);
    if (arr.size() != 2) {
        assert(false);
        // time_point默认值是0，所以可以返回
        return std::chrono::system_clock::time_point();
    }

    std::vector<CptString> date = stringutil::Split(arr[0], TEXT('-'));
    // 拆分字符串 yyyy-mm-dd 
    if (date.size() != 3) {
        assert(false);
        return std::chrono::system_clock::time_point();
    }

    tm t = { 0 };
    t.tm_year = std::stoi(date[0]) - 1900;
    t.tm_mon = std::stoi(date[1]) - 1;
    t.tm_mday = std::stoi(date[2]);

    time_t timestamp = mktime(&t);
    return std::chrono::system_clock::from_time_t(timestamp);
}










