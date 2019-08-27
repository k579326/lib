
include_directories(include)
add_library(cp-file STATIC dir_op.c file_op.c path_op.c)

target_link_libraries(cp-file dl)
