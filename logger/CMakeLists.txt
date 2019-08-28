

cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD "11")

add_executable(logger logger.cc logger.h logger_define.h
            core/printer.h core/printer.cc)





