

cmake_minimum_required(VERSION 3.8)

project(chsetutil)

set(CMAKE_CXX_STANDARD 11)

add_library(chsetutil STATIC util_chset_cvt.h util_chset_cvt.cc)

if (DEFINED RUNTEST)
    add_subdirectory(test)
endif ()

