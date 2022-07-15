cmake_minimum_required(VERSION 3.10)

set(MAPPING_SRC )
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
set(MAPPING_SRC ${MAPPING_SRC} win/filemapping.cc)
else()
set(MAPPING_SRC ${MAPPING_SRC} unix/filemapping.cc)
endif()

add_library(fmap STATIC ${MAPPING_SRC})
target_include_directories(fmap PUBLIC ./)
