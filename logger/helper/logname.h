
#ifndef _LOG_NAME_H_
#define _LOG_NAME_H_


#include <string>
#include <chrono>

class LogNameManager
{
public:
    LogNameManager(const std::string& path = "", const std::string& label = "");
    ~LogNameManager();
    
    void SetPath(const std::string& path);
    void SetLabel(const std::string& label);

    std::string GetLogName() const;
    bool NeedUpdate() const;

private:   
    std::string GetLogName(const std::chrono::system_clock::time_point& timepoint) const;
private:

    std::string path_;
    std::string label_;
};






























#endif // _LOG_NAME_H_




