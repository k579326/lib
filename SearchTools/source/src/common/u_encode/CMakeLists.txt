

set(CODESRC  check_encode.h check_encode.cc
			encode_convert.h encode_def.h 
			platform_codecvt.h platform_codecvt.cc)

add_library(encode STATIC ${CODESRC})

target_include_directories(encode INTERFACE ../)




