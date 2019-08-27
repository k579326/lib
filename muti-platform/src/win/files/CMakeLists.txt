add_library(cp-file STATIC dir_op.c file_op.c path_op.c)

target_link_libraries(cp-file Shlwapi Shell32 other)

set_target_properties(cp-file PROPERTIES FOLDER "multi-platform")