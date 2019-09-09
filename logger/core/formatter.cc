

#include "formatter.h"

#include <map>

const static std::map<int, LogLevels> g_column_config = {
    kDateTime, kDebugLevel,
    kThreadId, 0,
    kLogLevel, 0,
    kFileName, 0,

};




Formatter::Formatter(int column) : column_(column)
{
}

Formatter::~Formatter() { }

std::string Formatter::Format(LogLevels level, const std::string& filename, const std::string& function, int linenum)
{

}

void Formatter::AdjustOutputColumn(LogLevels level)
{



}