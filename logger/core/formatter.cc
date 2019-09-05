

#include "formatter.h"


Formatter::Formatter(int column) : column_(column)
{
}

Formatter::~Formatter() { }

std::string Formatter::Format(LogLevels level, const std::string& filename, const std::string& function, int linenum)
{

}
