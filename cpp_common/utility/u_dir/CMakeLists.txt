

cmake_minimum_required(VERSION 3.8)

project(udir)

add_library(udir STATIC util_dir.c util_dir.h)

if (DEFINED RUNTEST)
	add_subdirectory(test)
endif()