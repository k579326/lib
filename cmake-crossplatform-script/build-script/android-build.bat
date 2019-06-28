
set ANDROID_NDK=F:\android_sdk\ndk-bundle
set CURRENT_TIME=%date:~0,4%%date:~5,2%%date:~8,2%%time:~0,2%%time:~3,2%%time:~6,2%

::FOR %%A in (arm64-v8a armeabi-v7a armeabi mips mips64 x86 x86_64) do (
FOR %%A in (arm64-v8a armeabi-v7a armeabi) do (

mkdir %%A
pushd %cd%\%%A

set INCLUDE=0
set LIB=0
set LIBPATH=0

cmake ../.. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DDOE_RELEASE_FLAG=ON -DDOE_ENV_RUNNING=1 -DINSTALL_PATH="%cd%/AndroidLibrary" -DCMAKE_TOOLCHAIN_FILE="../../cmake-toolchain-file/android-cmake-master/android.toolchain.cmake" -DANDROID_NATIVE_API_LEVEL=14 -DANDROID_ABI=%%A -DDOE_BUILD_TIME=%CURRENT_TIME%
nmake
cd ..

)