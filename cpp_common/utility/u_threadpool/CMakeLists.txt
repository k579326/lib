


cmake_minimum_required(VERSION 3.8)


project(threadpool)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD "11")

add_library(threadpool STATIC util_threadpool.h util_threadpool.cc)
set_target_properties(threadpool PROPERTIES FOLDER "common")
target_include_directories(threadpool INTERFACE ../)

if (NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(threadpool pthread)
endif()

if (DEFINED RUNTEST)
    add_subdirectory(test)
endif()