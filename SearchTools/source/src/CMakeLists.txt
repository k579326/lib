cmake_minimum_required(VERSION 3.10)

include_directories(./kmp)

add_library(StringFinder STATIC finder.cc finder.h filter.h kmp/KMP_EX.h)
target_link_libraries(StringFinder fetch encode pathutil fileutil stringutil threadpool logger mapping)
target_include_directories(StringFinder PUBLIC . ./filter ./kmp)

add_subdirectory(platform)
add_subdirectory(common)
