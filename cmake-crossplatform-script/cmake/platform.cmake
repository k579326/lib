

cmake_minimum_required(VERSION 3.10)

# T_ means Target platform, diffent from host platform

# CPU type is ARM?
set(T_ARM OFF)

# target cpu arch name. 
# for android, it can be one of {"x86","x64","armeabi-v7a","arm64-v8a"}.
# for windows, it can be "x86" or "x64" only.
# for linux, same to windows, not support arm linux currently.
# for ios, it can be one of {"x86_64" "i386" "armv7" "armv7s" "arm64"}
set(T_ARCH "need set")

# CPU type is Intel?
set(T_IA OFF)

# check bits of target cpu 
# only can be 32 or 64
if(CMAKE_SIZEOF_VOID_P STREQUAL 4)
	set(T_BITS 32)
elseif(CMAKE_SIZEOF_VOID_P STREQUAL 8)
	set(T_BITS 64)
else()
	message(FATAL_ERROR "[CheckCPUBits] size of pointer is ${CMAKE_SIZEOF_VOID_P}")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(IS_WINDOWS ON)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(IS_LINUX ON)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(IS_ANDROID ON)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(IS_MACOS ON)
elseif(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(IS_IOS ON)
endif()

if(IS_WINDOWS)
    set(IS_POSIX OFF)
else()
    set(IS_POSIX ON)
endif()

if(IS_ANDROID)
	if(ANDROID_ABI STREQUAL "x86")
		set(T_ARM OFF)
		set(T_IA ON)
	elseif(ANDROID_ABI STREQUAL "x86_64")
		set(T_ARM OFF)
		set(T_IA ON)
	elseif(ANDROID_ABI STREQUAL "arm64-v8a")
		set(T_ARM ON)
		set(T_IA OFF)
	elseif(ANDROID_ABI STREQUAL "armeabi-v7a")
		set(T_ARM ON)
		set(T_IA OFF)
	else()
		message(FATAL_ERROR "target CPU arch ${ANDROID_ABI} not be supported!")
	endif()
	set(T_ARCH ${ANDROID_ABI})
	
elseif(IS_WINDOWS)
	set(T_ARM OFF)
	set(T_IA ON)
	if(CMAKE_SIZEOF_VOID_P STREQUAL 8)
		set(T_ARCH "x64")
	elseif(CMAKE_SIZEOF_VOID_P STREQUAL 4)
		set(T_ARCH "x86")
	endif()
elseif(IS_LINUX)
	# not support arm linux
	set(T_ARM OFF)
	set(T_IA ON)
	if(CMAKE_SIZEOF_VOID_P STREQUAL 8)
		set(T_ARCH "x64")
	elseif(CMAKE_SIZEOF_VOID_P STREQUAL 4)
		set(T_ARCH "x86")
	endif()
elseif(IS_IOS)
	message(FATAL_ERROR "TODO")
endif()

message(STATUS "----------------------------")
message(STATUS "[Host System]:  ${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS "[Target System]: ${CMAKE_SYSTEM_NAME}")
message(STATUS "[Target CPU Arch]: ${T_ARCH}")
if (T_ARM)
	message(STATUS "[Target CPU Type]: Arm")
endif()
if(T_IA)
	message(STATUS "[Target CPU Type]: Intel")
endif()

message(STATUS "[Target CPU Bits]: ${T_BITS}")
message(STATUS "----------------------------")
