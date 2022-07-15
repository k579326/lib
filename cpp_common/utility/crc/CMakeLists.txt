

cmake_minimum_required(VERSION 3.10)


add_library(crc STATIC checksum.h crc8.c crc16.c)
target_include_directories(crc PRIVATE ./)