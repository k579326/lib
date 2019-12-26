#ifndef _LOGGER_FORMATTER_H_
#define _LOGGER_FORMATTER_H_

#include <string>
#include <map>

#include "logger_define.h"


class Formatter
{
public:

    Formatter(int columon, std::string version);
    ~Formatter();

    std::string Format(LogLevels level, const std::string& filename, const std::string& function, int linenum) const;
private:

    int AdjustOutputColumn(LogLevels level) const;

private:
    int         column_;
    std::string version_;
};



#endif // _LOGGER_FORMATTER_H_

