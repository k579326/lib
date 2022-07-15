
#ifndef _LOG_NAME_H_
#define _LOG_NAME_H_


#include <string>
#include <chrono>

#include <cpt-string.h>


class LogNameManager
{
public:
    LogNameManager() : path_(TEXT("")), label_(TEXT("")), separator_(TEXT('#'))
    {
    }
    LogNameManager(const CptString& path, const CptString& label);
    ~LogNameManager();
    
    void SetPath(const CptString& path);
    void SetLabel(const CptString& label);

    CptString GetLogName() const;
    bool NeedUpdate() const;

    std::chrono::system_clock::time_point
    GetDateOfLogName(const CptString& logname) const;

private:   
    CptString GetLogName(const std::chrono::system_clock::time_point& timepoint) const;
private:

    CptString       path_;
    CptString       label_;
    const CptChar   separator_;
};






























#endif // _LOG_NAME_H_




