
@echo off

setlocal

set scriptname=%~n0
set script_dir=%~dp0
set sourcecode_dir=%~1
set build_dir=%~2
set VSNUM=%~3
set VSPLF=%~4
set VSBT=%~5

set vs_tar_num=%VSNUM%
set vs_tar_platform=%VSPLF%

set ERR=%scriptname.bat%: 

set cmake_command="%cmakepath%" -G"NMake Makefiles" -S"%sourcecode_dir%" -B"%build_dir%" -DCMAKE_BUILD_TYPE=%VSBT% -DOPENLOG=ON


pushd "%build_dir%"
echo =================== load visual studio environment ====================
call %script_dir%LoadVsEnv.bat %vs_tar_num% %vs_tar_platform%
if not %errorlevel%==0 (
	echo %ERR% environment is not be load correctly!
	exit -1
)

rem remove cmake cache file
rem rmdir /Q /S "%build_dir%/CMakeFiles"
rem del /Q /S "%build_dir%\\CMakeCache.txt"
echo =================== cmake build NMake project ====================
call %cmake_command%
if not %errorlevel%==0 (
	echo %ERR% cmake build failed!
	exit -1
)

rem do compile
echo =================== compile ====================
call nmake
if not %errorlevel%==0 (
	echo %ERR% nmake command failed!
	call nmake clean
	exit -1
)

endlocal
