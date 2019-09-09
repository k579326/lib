#ifndef _LOGGER_FORMATTER_H_
#define _LOGGER_FORMATTER_H_

#include <string>

#include "logger_define.h"

class Formatter
{
public:

    Formatter(int columon);
    ~Formatter();

    std::string Format(LogLevels level, const std::string& filename, const std::string& function, int linenum);
private:

    void AdjustOutputColumn(LogLevels level);

private:
    int column_;
};



#endif // _LOGGER_FORMATTER_H_

