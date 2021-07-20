


add_executable(test_encode test.cc)
target_link_libraries(test_encode encode dirutil)
target_include_directories(test_encode PRIVATE ../u_encode)
