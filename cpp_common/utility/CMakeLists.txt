
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

include_directories(../include)
include_directories(./)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	add_definitions(-DUNICODE)
endif()

add_subdirectory(dir)
add_subdirectory(encode)
add_subdirectory(file)
add_subdirectory(log)
#add_subdirectory(path)
add_subdirectory(string)
add_subdirectory(threadpool)
add_subdirectory(aes)
add_subdirectory(mmp)
add_subdirectory(crc)


add_subdirectory(test)

