cmake_minimum_required(VERSION 3.8)

project(pathutil)

set(CMAKE_CXX_STANDARD 11)
add_library(pathutil util_path.h util_path.cc)
set_target_properties(pathutil PROPERTIES FOLDER "common/")

if (DEFINED RUNTEST)
    add_subdirectory(test)
endif ()
