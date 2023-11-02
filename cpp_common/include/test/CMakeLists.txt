

cmake_minimum_required(VERSION 3.8)

include_directories(../)

add_executable(sem_test test_cc_semaphore.cc ../cc_semaphore.h)
add_executable(spinlock_test spinlock_test.cc)
