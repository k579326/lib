
add_executable(ftest file_test.c)

include_directories(../include)
target_link_libraries(ftest cp-file other)
set_target_properties(ftest PROPERTIES FOLDER "multi-platform/test")
#set_target_properties(ftest PROPERTIES FRAMEWORK TRUE FRAMEWORK_VERSION A MACOSX_FRAMEWORK_IDENTIFIER  "1221212")
