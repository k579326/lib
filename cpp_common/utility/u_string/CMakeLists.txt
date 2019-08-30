

cmake_minimum_required(VERSION 3.8)


project(stringutil)

add_library(stringutil util_string.h util_string.cc)

if (DEFINED RUNTEST)
    add_subdirectory(test)
endif()

