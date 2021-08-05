

cmake_minimum_required(VERSION 3.8)

project(fileutil)

set(CMAKE_CXX_STANDARD 11)

add_library(fileutil util_file.h win_file.cc)
set_target_properties(fileutil PROPERTIES FOLDER "common/")

set(EXTLIB )
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
set(EXTLIB Shlwapi.lib)
endif()

target_link_libraries(fileutil pathutil ${EXTLIB})
target_include_directories(fileutil INTERFACE ../)

if (DEFINED RUNTEST)
    add_subdirectory(test)
endif ()

