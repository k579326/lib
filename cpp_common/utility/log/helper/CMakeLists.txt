

cmake_minimum_required(VERSION 3.8)


add_library(loghelper STATIC logname.cc logname.h)
target_link_libraries(loghelper stringutil)
set_target_properties(loghelper PROPERTIES FOLDER "common/log")


