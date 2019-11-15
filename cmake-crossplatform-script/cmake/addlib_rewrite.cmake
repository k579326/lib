
# add_library，每次创建一个lib，都加入到变量PATCHERSDK_DEPLIBS中

macro(add_library target)
    _add_library(${target} ${ARGN})
    set(PATCHERSDK_DEPLIBS "${PATCHERSDK_DEPLIBS};${target}" CACHE INTERNAL "")
    #message(STATUS "${PATCHERSDK_DEPLIBS}")
endmacro()
