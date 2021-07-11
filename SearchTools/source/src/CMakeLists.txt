cmake_minimum_required(VERSION 3.10)

include_directories(./common)
include_directories(./kmp)

add_library(StringFinder STATIC finder.cc finder.h filter.h)
target_link_libraries(StringFinder fetch)
target_include_directories(StringFinder PUBLIC . ./filter)

add_subdirectory(platform)
add_subdirectory(common)