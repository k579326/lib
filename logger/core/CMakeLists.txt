

cmake_minimum_required(VERSION 3.8)

add_library(logcore STATIC printer.h printer.cc
            formatter.h formatter.cc io.cc ./io.h)
