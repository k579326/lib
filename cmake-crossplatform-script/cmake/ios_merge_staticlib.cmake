
# iOS合并所有静态库
# 其他平台忽略此脚本

if ((${IS_IOS}) AND (NOT(CMAKE_GENERATOR MATCHES "Xcode")))
	
    list(APPEND CMAKE_FIND_LIBRARY_PREFIXES lib)
    list(APPEND CMAKE_FIND_LIBRARY_SUFFIXES .a)
	
    set(alllibexps "")
    foreach(eachlib ${GLOBAL_DEPENDENTLIB})
        set(alllibexps ${alllibexps} $<TARGET_FILE:${eachlib}>)
    endforeach()

    # add openssl to merge list
    #foreach(eachlib ${openssl_lib})
        # 因为与app自身的openssl库引发冲突，ios不再内部链接openssl库
        # set(alllibexps ${alllibexps} ${openssl_link_path}/lib${eachlib}.a)
    #endforeach()

    # add libcurl to merge list
    #find_library(curllibpath ${curl_lib} ${curl_link_path}/lib)
    #set(alllibexps ${alllibexps} ${curl_link_path}/lib${curl_lib}.a)


    # Framework Name
    set(FrameworkName ${SDKNAME}_${CMAKE_OSX_ARCHITECTURES})
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/empty.c "void empyt(){}")
    add_library(${FrameworkName} STATIC ${CMAKE_CURRENT_BINARY_DIR}/empty.c
	${CMAKE_SOURCE_DIR}/inc/logdefine.h
	${CMAKE_SOURCE_DIR}/inc/debug_log.h
	)
    set_target_properties(${FrameworkName} PROPERTIES
        FRAMEWORK TRUE
        FRAMEWORK_VERSION C
        MACOSX_FRAMEWORK_IDENTIFIER "com.pwrd.debug.native"
        VERSION 1.0.0.0
	SOVERSION 1.0.0.0
	PUBLIC_HEADER "${CMAKE_SOURCE_DIR}/inc/logdefine.h;${CMAKE_SOURCE_DIR}/inc/debug_log.h"
	XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"	
    )
    
    set(TMPLIBNAME "${SDKNAME}_tmp")
    add_custom_command(TARGET ${FrameworkName} PRE_LINK
        COMMAND rm -f $<TARGET_FILE:${FrameworkName}>
        COMMAND rm -f ${TMPLIBNAME}
        COMMAND /usr/bin/libtool -static -o ${CMAKE_BINARY_DIR}/${TMPLIBNAME} ${alllibexps}
    )

    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/copy_framework.c "void target_copy_framework(){}")
    add_library(CopyFramework STATIC ${CMAKE_CURRENT_BINARY_DIR}/copy_framework.c)
    target_link_libraries(CopyFramework ${FrameworkName})

    add_custom_command(TARGET CopyFramework POST_BUILD
	COMMAND /bin/cp -rf ${CMAKE_BINARY_DIR}/${TMPLIBNAME} $<TARGET_FILE:${FrameworkName}>
	COMMAND /bin/mkdir -p ${CMAKE_BINARY_DIR}/../Framework/${CMAKE_IOS_TARGET_PLATFORM}
        COMMAND /bin/cp -rf $<TARGET_FILE_DIR:${FrameworkName}> ${CMAKE_BINARY_DIR}/../Framework/${CMAKE_IOS_TARGET_PLATFORM}
    )

endif ()
