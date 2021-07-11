cmake_minimum_required(VERSION 3.10)

set(CVTSRC filefetch.cc filefetch.h)
set(MAPSRC filemapping.h)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND CVTSRC  win/convert/character_convert.cc 
                        win/convert/character_convert.h)
    list(APPEND MAPSRC  win/filemapping.cc)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND MAPSRC  unix/filemapping.cc)
endif()


include_directories(./)

add_library(fetch STATIC ${CVTSRC})
target_link_libraries(fetch common)


add_library(mapping STATIC ${MAPSRC})
target_link_libraries(mapping common)
