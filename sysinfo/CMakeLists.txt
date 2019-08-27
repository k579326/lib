project(cb_sysinfo)
cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

set(SRCDIR ${CUSTOM_OS_NAME})

if (CUSTOM_OS_WIN)
    set (DEPENDLIBS version)
elseif (CUSTOM_OS_MAC)
    set (DEPENDLIBS "-framework CoreServices")
endif()

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/${SRCDIR} SRCFILE)
add_library(cb_sysinfo STATIC ${SRCFILE})
target_link_libraries(cb_sysinfo ${DEPENDLIBS})

set_target_properties(cb_sysinfo PROPERTIES FOLDER "third")



