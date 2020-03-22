cmake_minimum_required(VERSION 3.8)

project(urlutil)

add_library(urlutil STATIC util_url.cc util_url.h)
set_target_properties(urlutil PROPERTIES FOLDER "common/")

