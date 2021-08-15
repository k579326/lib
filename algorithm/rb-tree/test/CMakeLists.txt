cmake_minimum_required(VERSION 3.10)


project(testRbTree)

add_executable(testRbTree test.cc)
target_link_libraries(testRbTree rbtree)
target_include_directories(testRbTree PRIVATE ../)