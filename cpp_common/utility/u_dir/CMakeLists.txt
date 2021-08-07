

cmake_minimum_required(VERSION 3.8)

project(udir)

add_library(dirutil STATIC util_dir.c util_dir.h)

target_link_libraries(dirutil)
set_target_properties(dirutil PROPERTIES FOLDER "common/")
target_include_directories(dirutil INTERFACE ../)

if (DEFINED RUNTEST)
	add_subdirectory(test)
endif()