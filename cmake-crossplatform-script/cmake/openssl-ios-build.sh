#!/bin/bash

# DIESCRIPTION: build openssl for ios
# environment variable must be set before configure
#       export CROSS_TOP=/path/of/sdk
#       export CROSS_SDK=sdkname.sdk
#       export CC='gcc/clang'

# 1. $ ./configure ios-cross
#   config makefile for building openssl on ios (armv7)


# 2. $ ./configure ios64-cross
#   config makefile for building openssl on ios (arm64)


# 3. $ ./configure iphoneos-cross
#    if variable CC been set to 'gcc -arch arm64', this command will be same to ./configure ios64-cross
#    you can change ios sdk min version by setting CC to "gcc -mios-version-min=8.0"

VERSION_MIN=9.0
ARCHS="i386 x86_64 armv7 armv7s arm64"

# directory
ROOTDIR=iOSBin
FATDIR=${ROOTDIR}/fat

mkdir -p ${ROOTDIR}
mkdir -p ${FATDIR}

for ARCH in ${ARCHS}
do
  if [[ "$ARCH" == arm* ]]; then
    PLATFORM="iPhoneOS"
    MINVER_KEY="iphoneos-version-min"
  else
    PLATFORM="iPhoneSimulator"
    MINVER_KEY="ios-simulator-version-min"
  fi


  if [[ "${VERSION_MIN}" == 9.* || "${VERSION_MIN}" == [0-9][0-9].* ]]; then
    LOCAL_CONFIG_OPTIONS="${LOCAL_CONFIG_OPTIONS} -fembed-bitcode"
  fi


  export CROSS_TOP=/Applications/Xcode.app/Contents/Developer/Platforms/${PLATFORM}.platform/Developer
  export CROSS_SDK=${PLATFORM}.sdk
  export CC="clang -arch ${ARCH} -m${MINVER_KEY}=${VERSION_MIN} ${LOCAL_CONFIG_OPTIONS}"

  mkdir -p ${ROOTDIR}/${ARCH}/lib/
  make clean

  if [ ["$1" == arm*] ]; then
    ./Configure iphoneos-cross no-shared
  else
    ./Configure iphoneos-cross no-shared no-asm
  fi

  make

  mkdir -p ${ROOTDIR}/${ARCH}/include/openssl
  cp ./libssl.a ${ROOTDIR}/${ARCH}/lib/
  cp ./libcrypto.a ${ROOTDIR}/${ARCH}/lib/
  cp -r ./include/openssl/* ${ROOTDIR}/${ARCH}/include/openssl

  make clean
done




