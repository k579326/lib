cmake_minimum_required(VERSION 3.10)

include_directories(../source/src)

set(CMAKE_CXX_STANDARD 14)

add_executable(st_test test.cc)
target_link_libraries(st_test StringFinder)


