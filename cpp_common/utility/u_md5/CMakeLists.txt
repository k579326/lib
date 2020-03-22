
cmake_minimum_required(VERSION 3.8)

project(md5util)


set(CMAKE_CXX_STANDARD 11)
add_library(md5util util_md5.h util_md5.cc)

target_link_libraries(md5util md5 stringutil)
set_target_properties(md5util PROPERTIES FOLDER "common")
