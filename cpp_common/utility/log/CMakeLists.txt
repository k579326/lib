

cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

include_directories(../)
include_directories(./)

add_subdirectory(helper)
add_subdirectory(autoclear)
add_subdirectory(core)

add_library(logger STATIC logger.cc logger.h logger_define.h stringdef/stringdef.cc stringdef/stringdef.h)
set_target_properties(logger PROPERTIES FOLDER "common/log")

# use option variable, set OFF default, you can do "cmake -DOPENLOG=ON .." if want to open it .
option(OPENLOG "this variable will be cache on building, if need changing this variable, must clean and re-cmake" OFF)
if (OPENLOG)
	# macro _ENABLE_LOGGER_ decide whether the logger will be open
	target_compile_definitions(logger INTERFACE _ENABLE_LOGGER_)
	message("[module-logger] logger is being opened!")
endif()


set(OutLib)
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set(OutLib encode)
else ()
	set(OutLib pthread)
endif ()
target_link_libraries(logger logcore loghelper logclr dirutil fileutil pathutil ${OutLib})
target_include_directories(logger PUBLIC ./)





