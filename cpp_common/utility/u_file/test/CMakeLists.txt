cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 11)

add_executable(filetest test.cc)
target_include_directories(filetest PUBLIC ${fileutil_SOURCE_DIR})
target_link_libraries(filetest PUBLIC fileutil)
