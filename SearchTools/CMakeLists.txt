cmake_minimum_required(VERSION 3.10)

add_definitions(-DUNICODE)

include_directories(source)
include_directories(source/include)

add_subdirectory(source)
add_subdirectory(test)
