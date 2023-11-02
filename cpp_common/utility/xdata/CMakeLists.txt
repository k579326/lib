cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)


include_directories(./ ./_xdata)

set(SRCLIST xdata.cc
            xdata.h
            filesource.cc
            filesource.h
            _xdata/dataencoder.cc
            _xdata/dataencoder.h
            _xdata/datasource.h
            _xdata/datasync.h
            _xdata/datasync.cc)

add_library(xdata STATIC ${SRCLIST})

target_link_libraries(xdata fileutil fmap)

add_executable(xdata_test test_data_type.cc)
target_link_libraries(xdata_test xdata)


