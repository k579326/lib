
cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 11)
add_executable(test_threadpool test.cc)

target_link_libraries(test_threadpool threadpool)
target_include_directories(test_threadpool PRIVATE ${threadpool_SOURCE_DIR})

