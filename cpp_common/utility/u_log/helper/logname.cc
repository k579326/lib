
#include "logname.h"

#include <assert.h>

#include "u_file/util_file.h"
#include "u_string/util_string.h"

LogNameManager::LogNameManager(const std::string& path, const std::string& label) : path_(path), label_(label), separator_('#')
{
}

LogNameManager::~LogNameManager()
{

}

void LogNameManager::SetPath(const std::string& path)
{
    path_ = path;
}
void LogNameManager::SetLabel(const std::string& label)
{
    label_ = label;
}


std::string LogNameManager::GetLogName() const
{
    return GetLogName(std::chrono::system_clock::now());
}

bool LogNameManager::NeedUpdate() const
{
    return !fileutil::File::IsExist(GetLogName());
}

std::string LogNameManager::GetLogName(const std::chrono::system_clock::time_point& timepoint) const
{
    time_t timestamp = std::chrono::system_clock::to_time_t(timepoint);

    char timestr[64];
    tm* tp = localtime(&timestamp);
    sprintf(timestr, "%4d-%02d-%02d", tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday);

    std::string filename = std::string(timestr) + separator_ + label_;
    std::string fullpath = path_ + "/" + filename + ".log";

    return fullpath;
}


std::chrono::system_clock::time_point
LogNameManager::GetDateOfLogName(const std::string& logname) const
{
    // 拆分 {date}#{gamename}.log
    std::vector<std::string> arr = stringutil::Split(logname, separator_);
    if (arr.size() != 2) {
        assert(false);
        // time_point默认值是0，所以可以返回
        return std::chrono::system_clock::time_point();
    }

    std::vector<std::string> date = stringutil::Split(arr[0], '-');
    // 拆分字符串 yyyy-mm-dd 
    if (date.size() != 3) {
        assert(false);
        return std::chrono::system_clock::time_point();
    }

    tm t = { 0 };
    t.tm_year = std::atoi(date[0].c_str()) - 1900;
    t.tm_mon = std::atoi(date[1].c_str()) - 1;
    t.tm_mday = std::atoi(date[2].c_str());

    time_t timestamp = mktime(&t);
    return std::chrono::system_clock::from_time_t(timestamp);
}










