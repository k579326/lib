


cmake_minimum_required(VERSION 3.8)


project(threadpool)

set(CMAKE_CXX_STANDARD 11)
add_library(threadpool STATIC util_threadpool.h util_threadpool.cc)

if (DEFINED RUNTEST)
    add_subdirectory(test)
endif()