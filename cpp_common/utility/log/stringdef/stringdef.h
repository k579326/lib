#ifndef _STRING_DEFINE_H_
#define _STRING_DEFINE_H_


#include <map>
#include <string>

#include "cpt-string.h"
#include "logger_define.h"

extern const std::map<LogLevels, CptString> g_loglevel_str;
extern const std::map<RunModel, CptString> g_runmodel_str;


#endif // _STRING_DEFINE_H_


