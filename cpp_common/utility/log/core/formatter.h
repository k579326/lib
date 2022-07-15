#ifndef _LOGGER_FORMATTER_H_
#define _LOGGER_FORMATTER_H_

#include <string>
#include <map>

#include "logger_define.h"
#include "cpt-string.h"


class Formatter
{
public:

    Formatter(int columon, const CptString& version);
    ~Formatter();

    CptString Format(LogLevels level, const CptString& filename, const CptString& function, int linenum) const;
private:

    int AdjustOutputColumn(LogLevels level) const;

private:
    int         column_;
    CptString   version_;
};



#endif // _LOGGER_FORMATTER_H_

