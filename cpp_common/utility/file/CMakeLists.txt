

cmake_minimum_required(VERSION 3.8)

    
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

project(fileutil)

set(FILESRC )
set(EXTLIB )
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(FILESRC win_file.cc)
    set(EXTLIB Shlwapi.lib)
else ()
    set(FILESRC unix_file.cc)
endif()

add_library(fileutil util_file.h ${FILESRC})
set_target_properties(fileutil PROPERTIES FOLDER "common/")



target_link_libraries(fileutil pathutil ${EXTLIB})
target_include_directories(fileutil INTERFACE ../)

if (DEFINED RUNTEST)
    add_subdirectory(test)
endif ()

