cmake_minimum_required(VERSION 3.10)

include_directories(../source/src)

set(CMAKE_CXX_STANDARD 14)

add_executable(grep test.cc)
target_link_libraries(grep StringFinder)


