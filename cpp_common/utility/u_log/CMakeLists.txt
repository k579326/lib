

cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD "11")

include_directories(.)

add_subdirectory(helper)
add_subdirectory(autoclear)
add_subdirectory(core)

add_library(logger STATIC logger.cc logger.h logger_define.h stringdef/stringdef.cc stringdef/stringdef.h)
set_target_properties(logger PROPERTIES FOLDER "common/log")

target_link_libraries(logger logcore loghelper logclr)
target_include_directories(logger PRIVATE .)





