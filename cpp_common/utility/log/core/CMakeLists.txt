

cmake_minimum_required(VERSION 3.8)

add_subdirectory(console)

add_library(logcore STATIC printer.h printer.cc
            formatter.h formatter.cc io.cc ./io.h)
target_link_libraries(logcore console_ex fileutil fmap)
set_target_properties(logcore PROPERTIES FOLDER "common/log")


