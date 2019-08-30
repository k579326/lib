

cmake_minimum_required(VERSION 3.8)

project(fileutil)

add_library(fileutil util_file.cc util_file.h)

if (DEFINED RUNTEST)
    add_subdirectory(test)
endif ()

