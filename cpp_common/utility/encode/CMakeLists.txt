
cmake_minimum_required(VERSION 3.10)

project(encode)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

set(CODESRC  check_encode.h check_encode.cc
			encode_convert.h encode_def.h 
			platform_codecvt.h platform_codecvt.cc)

add_library(encode STATIC ${CODESRC})

target_include_directories(encode INTERFACE ../)




