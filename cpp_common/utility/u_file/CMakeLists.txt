

cmake_minimum_required(VERSION 3.8)

project(fileutil)

set(CMAKE_CXX_STANDARD 11)

add_library(fileutil util_file.cc util_file.h)

if (DEFINED RUNTEST)
    add_subdirectory(test)
endif ()

