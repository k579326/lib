

cmake_minimum_required(VERSION 3.8)

add_definitions(-D_ENABLE_LOGGER_)

add_executable(logtest testlog.cc)
target_link_libraries(logtest logger)

