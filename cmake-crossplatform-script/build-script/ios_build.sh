#!/bin/sh

#compile data_auth static lib for platforms —- 
#i386 x86_64 armv7 armv7s arm64

arch=(arm64s x86_64 i386 armv7 arm64)

for var in ${arch[@]};
do
    mkdir ${var}
    cd ${var}

    #if [[ “${var}” = “i386” || “${var}” = “x86_64” ]];then
	#    IOS_OS=iPhoneSimulator
    #else 
	#    IOS_OS=iPhoneOS
    #fi

    # IOS_ROOT=/Applications/Xcode.app/Contents/Developer/Platforms/${IOS_OS}.platform/Developer/
    if [[ “${var}” = “i386” || “${var}” = “x86_64” || “${var}” = “arm64s” ]]; then
        BitCodeFlag=OFF
        iOS_Platform=simulator
    else
        BitCodeFlag=ON
        iOS_Platform=iphoneos
    fi

    if [[ "${var}" = "arm64s" ]]; then
        var=arm64
    fi
    
    
    # 设置环境变量，工具链文件会被cmake其他脚本调用，无法通过除CMAKE_OSX_ARCHITECTURES之外的参数传递信息
    unset iOS_PLATFORM
    export iOS_PLATFORM=${iOS_Platform}

    cmake -DCMAKE_TOOLCHAIN_FILE=../../cmake/ios.toolchain.cmake -DCMAKE_OSX_ARCHITECTURES=${var} -DCMAKE_BUILD_TYPE=Release -DENABLE_BITCODE=${BitCodeFlag} ../..

    make
    
    cd ..
done

#merge all libs with lipo command

rm -rf MyTest.xcframework

mkdir -p ./xcFramework/iphoneos
mkdir -p ./xcFramework/simulator

cp -rf ./Framework/iphoneos/MyTest_arm64.framework ./xcFramework/iphoneos
cp -rf ./Framework/simulator/MyTest_arm64.framework ./xcFramework/simulator

rm -rf ./xcFramework/iphoneos/MyTest.framework
rm -rf ./xcFramework/simulator/MyTest.framework

mv -f ./xcFramework/iphoneos/MyTest_arm64.framework ./xcFramework/iphoneos/MyTest.framework
mv -f ./xcFramework/simulator/MyTest_arm64.framework ./xcFramework/simulator/MyTest.framework

rm ./xcFramework/iphoneos/MyTest.framework/MyTest_arm64
rm ./xcFramework/simulator/MyTest.framework/MyTest_arm64

lipo -output ./xcFramework/iphoneos/MyTest.framework/MyTest -create ./Framework/iphoneos/MyTest_arm64.framework/MyTest_arm64 ./Framework/iphoneos/MyTest_armv7.framework/MyTest_armv7
lipo -output ./xcFramework/simulator/MyTest.framework/MyTest -create ./Framework/simulator/MyTest_i386.framework/MyTest_i386 ./Framework/simulator/MyTest_x86_64.framework/MyTest_x86_64 ./Framework/simulator/MyTest_arm64.framework/MyTest_arm64

xcodebuild -create-xcframework -framework ./xcFramework/iphoneos/MyTest.framework -framework ./xcFramework/simulator/MyTest.framework -output MyTest.xcframework

rm -rf ./xcFramework
rm -rf ./Framework

