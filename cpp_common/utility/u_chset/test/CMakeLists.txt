
cmake_minimum_required(VERSION 3.8)


add_executable(chsettest test.cc)

target_include_directories(chsettest PUBLIC ${chsetutil_SOURCE_DIR})
target_link_libraries(chsettest PUBLIC chsetutil)

