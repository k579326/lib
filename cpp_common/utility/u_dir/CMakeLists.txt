

cmake_minimum_required(VERSION 3.8)

project(udir)

set(CMAKE_CXX_STANDARD 11)
add_library(udir STATIC util_dir.c util_dir.h)

set_target_properties(udir PROPERTIES FOLDER "common/")
target_include_directories(udir INTERFACE ../)

if (DEFINED RUNTEST)
	add_subdirectory(test)
endif()