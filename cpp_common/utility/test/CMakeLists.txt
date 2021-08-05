
include_directories(../)

add_executable(test_encode test.cc)
target_link_libraries(test_encode encode dirutil)
target_include_directories(test_encode PRIVATE ../u_encode)

add_executable(test_log testlog.cc)
target_link_libraries(test_log logger)
target_include_directories(test_log PRIVATE ../)

add_executable(test_file test_file.cc)
target_link_libraries(test_file pathutil fileutil)