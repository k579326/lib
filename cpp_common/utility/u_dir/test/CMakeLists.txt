

cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 11)
add_executable(dirtest test.c)

target_include_directories(dirtest PUBLIC ${udir_SOURCE_DIR})
target_link_libraries(dirtest udir)