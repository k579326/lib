

include_directories(include)

add_definitions(-D_GNU_SOURCE)      # 添加GNU支持，否则有的linux数据结构都用不了
set(platform_src "")

IF(CUSTOM_OS_WIN)
    set(platform_src "src/win")
ELSEIF(CUSTOM_OS_ANDROID OR CUSTOM_OS_MAC OR CUSTOM_OS_IOS OR CUSTOM_OS_LINUX)
    set(platform_src "src/unix")
ELSE()
    message(FATAL_ERROR "target OS not support!")
ENDIF()

include_directories(${platform_src})
add_subdirectory(${platform_src})
add_subdirectory(test)