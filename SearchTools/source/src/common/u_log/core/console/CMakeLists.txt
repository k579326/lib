
cmake_minimum_required(VERSION 3.8)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set (SRC win-console.cc)
else ()
    set (SRC unix-console.cc)
endif ()

add_library(console_ex STATIC console_extern.h ${SRC})
set_target_properties(console_ex PROPERTIES FOLDER "common/log")


