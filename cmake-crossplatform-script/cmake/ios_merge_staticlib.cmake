
# iOS合并所有静态库
# 其他平台忽略此脚本

if (${PF_OS_IOS} AND NOT ${CMAKE_GENERATOR} MATCHES "Xcode")

    set(alllibexps "")
    foreach(eachlib ${PATCHERSDK_DEPLIBS})
        set(alllibexps ${alllibexps} $<TARGET_FILE:${eachlib}>)
    endforeach()

    #message(SATUS "${alllibexps}")

    set(TMPLIBNAME "${PATCHERSDK_LIBNAME}_tmp")
    add_custom_command(TARGET ${PATCHERSDK_LIBNAME} POST_BUILD
        COMMAND rm -f ${TMPLIBNAME}
        COMMAND /usr/bin/libtool -static -o ${TMPLIBNAME} ${alllibexps}
        COMMAND rm -f $<TARGET_FILE:${PATCHERSDK_LIBNAME}>
        COMMAND mv ${TMPLIBNAME} $<TARGET_FILE_NAME:${PATCHERSDK_LIBNAME}>
        COMMAND /bin/cp -f $<TARGET_FILE:${PATCHERSDK_LIBNAME}> ${CMAKE_BINARY_DIR}/
    )

endif ()

