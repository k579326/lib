

# iOS合并所有静态库
# 其他平台忽略此脚本

if ((${PF_OS_IOS}) AND (NOT(CMAKE_GENERATOR MATCHES "Xcode")))

    list(APPEND CMAKE_FIND_LIBRARY_PREFIXES lib)
    list(APPEND CMAKE_FIND_LIBRARY_SUFFIXES .a)

    set(alllibexps "")
    foreach(eachlib ${PATCHERSDK_DEPLIBS})
        set(alllibexps ${alllibexps} $<TARGET_FILE:${eachlib}>)
    endforeach()

    # add openssl to merge list
    foreach(eachlib ${openssl_lib})
        # 因为与app自身的openssl库引发冲突，ios不再内部链接openssl库
        # set(alllibexps ${alllibexps} ${openssl_link_path}/lib${eachlib}.a)
    endforeach()

    # add libcurl to merge list
    find_library(curllibpath ${curl_lib} ${curl_link_path}/lib)
    set(alllibexps ${alllibexps} ${curl_link_path}/lib${curl_lib}.a)

    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/empty.c "void empyt(){}")
    add_library(FrameworkLib STATIC ${CMAKE_CURRENT_BINARY_DIR}/empty.c
        ${CMAKE_SOURCE_DIR}/error.h
        ${CMAKE_SOURCE_DIR}/funcheader.h
    )
    set_target_properties(FrameworkLib PROPERTIES
        FRAMEWORK TRUE
        FRAMEWORK_VERSION C
        MACOSX_FRAMEWORK_IDENTIFIER "com.xxxx.yyyy.zzz"
        MACOSX_FRAMEWORK_BUNDLE_VERSION "1.0.0"
    )
    set_property(TARGET FrameworkLib PROPERTY
		PUBLIC_HEADER 
		"${CMAKE_SOURCE_DIR}/error.h;${CMAKE_SOURCE_DIR}/funcheader.h"
    )

    set(TMPLIBNAME "${ORIGIN_NAME}_tmp")
    add_custom_command(TARGET FrameworkLib PRE_LINK
        COMMAND rm -f $<TARGET_FILE:FrameworkLib>
        COMMAND rm -f ${TMPLIBNAME}
        COMMAND /usr/bin/libtool -static -o ${TMPLIBNAME} ${alllibexps}
        COMMAND /bin/cp -f ${CMAKE_CURRENT_BINARY_DIR}/${TMPLIBNAME} ${CMAKE_BINARY_DIR}/$<TARGET_FILE_NAME:${ORIGIN_NAME}>
    )

    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/copy_framework.c "void target_copy_framework(){}")
    add_library(CopyFramework STATIC ${CMAKE_CURRENT_BINARY_DIR}/copy_framework.c)
    target_link_libraries(CopyFramework FrameworkLib)

    add_custom_command(TARGET CopyFramework POST_BUILD
        COMMAND /bin/cp -rf $<TARGET_FILE_DIR:FrameworkLib> ${CMAKE_BINARY_DIR}/../
    )

endif ()


