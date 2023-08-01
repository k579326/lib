cmake_minimum_required(VERSION 3.10)

set(MAPPING_SRC )
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
set(MAPPING_SRC ${MAPPING_SRC} win/memory_mapping.cc)
else()
set(MAPPING_SRC ${MAPPING_SRC} unix/memory_mapping.cc)
endif()

add_library(fmap STATIC ${MAPPING_SRC} memory_mapping.h)
target_include_directories(fmap PUBLIC ./)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
target_link_libraries(fmap shlwapi)
endif()