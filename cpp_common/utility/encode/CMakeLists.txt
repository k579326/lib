

cmake_minimum_required(VERSION 3.10)

project(encode)

add_library(encode STATIC platform_codecvt.cc platform_codecvt.h)
target_include_directories(encode INTERFACE ./)
set_target_properties(encode PROPERTIES FOLDER "common")
