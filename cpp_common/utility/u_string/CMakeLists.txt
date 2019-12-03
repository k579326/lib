

cmake_minimum_required(VERSION 3.8)


project(stringutil)

set(CMAKE_CXX_STANDARD 11)
add_library(stringutil util_string.h util_string.cc)
set_target_properties(stringutil PROPERTIES FOLDER "common/")

if (DEFINED RUNTEST)
    add_subdirectory(test)
endif()

