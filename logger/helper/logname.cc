
#include "logname.h"

#include "u_file/util_file.h"

LogNameManager::LogNameManager(const std::string& path, const std::string& label) : path_(path), label_(label)
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


std::string LogNameManager::GetLogName()
{
    return GetLogName(std::chrono::system_clock::now());
}

bool LogNameManager::NeedUpdate()
{
    return !fileutil::File::IsExist(GetLogName());
}

std::string LogNameManager::GetLogName(const std::chrono::system_clock::time_point& timepoint)
{
    time_t timestamp = std::chrono::system_clock::to_time_t(timepoint);

    char timestr[256];
    tm* tp = localtime(&timestamp);
    sprintf(timestr, "%4d-%02d-%02d ", tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday);

    std::string filename = std::string(timestr) + "#" + label_;
    std::string fullpath = path_ + "/" + filename + ".log";

    return fullpath;
}













