add_library(other other.c)

IF (NOT CUSTOM_OS_ANDROID)
	target_link_libraries(other pthread)
ENDIF()