#!/bin/sh

#compile data_auth static lib for platforms —- 
#i386 x86_64 armv7 armv7s arm64

platforms=(x86_64 i386 armv7 armv7s arm64)

for var in ${platforms[@]};
do
    mkdir ${var}
    cd ${var}
#   if [[ “${var}” = “i386” || “${var}” = “x86_64” ]];then
#	IOS_OS=iPhoneSimulator
#    else 
#	IOS_OS=iPhoneOS
#    fi

#    IOS_ROOT=/Applications/Xcode.app/Contents/Developer/Platforms/${IOS_OS}.platform/Developer/
    if [[ “${var}” = “i386” || “${var}” = “x86_64” ]]; then
        BitCodeFlag=OFF
    else
        BitCodeFlag=ON
    fi

    cmake -DCMAKE_TOOLCHAIN_FILE=../../cmake-toolchain-file/mac\&ios-cmake-toochain/ios_64.cmake -DCMAKE_OSX_ARCHITECTURES=${var} -DCMAKE_BUILD_TYPE=Release -DDOE_RELEASE_FLAG=ON -DDOE_ENV_RUNNING=1 -DENABLE_BITCODE=${BitCodeFlag} ../..

    make
    
    cd ..
done

#merge all libs with lipo command

libpath=API/src/libdata_auth_api.a

for index in ${!platforms[@]};
do
	liblist=${liblist}\ ${platforms[$index]}/${libpath}
	echo “${liblist}”
done
echo ${liblist}
lipo -create ${liblist} -output libdata_auth_api.a
