

cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD "11")

include_directories(.)

add_subdirectory(core)
add_subdirectory(helper)

add_library(logger STATIC logger.cc logger.h logger_define.h stringdef/stringdef.cc stringdef/stringdef.h)
target_link_libraries(logger logcore loghelper)
target_include_directories(logger PRIVATE .)





