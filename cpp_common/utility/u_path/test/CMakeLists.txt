

cmake_minimum_required(VERSION 3.8)

add_executable(pathtest test.cc)

target_include_directories(pathtest PUBLIC ${pathutil_SOURCE_DIR})
target_link_libraries(pathtest PUBLIC pathutil)

