#使用该工具链文件命令示例#
#
#cmake -DCMAKE_BUILD_TYPE=Debug \
#       -DCMAKE_C_COMPILER=/home/virbox/toolchain/gcc-linaro-5.5.0-2017.10-x86_64_arm-linux-gnueabi/bin/arm-linux-gnueabi-gcc \
#       -DCMAKE_CXX_COMPILER=/home/virbox/toolchain/gcc-linaro-5.5.0-2017.10-x86_64_arm-linux-gnueabi/bin/arm-linux-gnueabi-g++ \
#       -DLINUX_ARCH=arm-virbox
#       -DDOE_ENV_RUNNING=0 \
#       -DCMAKE_TOOLCHAIN_FILE=/home/virbox/ddoeclient/code/cmake-toolchain-file/linux-cmake-toolchain/linux_arm.cmake \
#       -DCMAKE_INSTALL_PREFIX=/home/virbox/rootfs/home/root/ddoe_client_test
#       -G "CodeBlocks - Unix Makefiles" \
#       ../code
#


SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR arm)

#后续依赖该变量寻找OpenSSL和Curl库,目前LINUX_ARCH的取值有：x86, x64, arm, arm-hisi, arm-virbox;#
#其中: arm-hisi为猫眼项目发布使用; arm-virbox在核心板开发板使用#
#SET(LINUX_ARCH arm)

SET(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_DIR})
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

#名称不一致问题,这里不设置在CMAKE命令里自己指定#
#SET(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/bin/arm-linux-gnueabi-gcc)
#SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/arm-linux-gnueabi-g++)

#SET(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/bin/arm-none-eabi-gcc)
#SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/arm-none-eabi-g++)

#静态链接标准库：开发板上的标准库和开发环境不一直,避免快速解决方案#
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")