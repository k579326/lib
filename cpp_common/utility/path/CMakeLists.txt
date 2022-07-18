cmake_minimum_required(VERSION 3.8)

project(pathutil)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

add_library(pathutil INTERFACE util_path.h)
set_target_properties(pathutil PROPERTIES FOLDER "common/")
target_include_directories(pathutil INTERFACE ../)

if (DEFINED RUNTEST)
    add_subdirectory(test)
endif ()
