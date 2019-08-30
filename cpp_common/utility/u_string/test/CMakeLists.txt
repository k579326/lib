

cmake_minimum_required(VERSION 3.8)


add_executable(stringtest test.cc)

target_include_directories(stringtest PUBLIC ${stringutil_SOURCE_DIR})
target_link_libraries(stringtest PUBLIC stringutil)


