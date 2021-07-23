

add_library(logclr logclear.cc logclear.h)

set_target_properties(logclr PROPERTIES FOLDER "common/log")
target_link_libraries(logclr udir)

