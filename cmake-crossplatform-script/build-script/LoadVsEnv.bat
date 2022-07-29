
@echo off

set scriptname=%~n0
set vs_num=%1
set platform=%2

set x64_dir=%ProgramFiles%
set x86_dir=%ProgramFiles(x86)%

set vs_installdir=

for %%i in ("%x86_dir%" "%x64_dir%") do (
	if exist "%%i/Microsoft Visual Studio/%vs_num%" (
		set vs_installdir=%%i
		break
	)
)

if not defined vs_installdir (
	echo %scriptname%.bat: Visual Studio %vs_num% not found, exit
	exit /B -1
)

set vs_installdir=%vs_installdir:"=%
set vs_installdir=%vs_installdir%/Microsoft Visual Studio/%vs_num%

set vs_ver=
for %%i in (Enterprise, Community, Professional) do (
	if exist "%vs_installdir%/%%i/" (
		set vs_ver=%%i/
		break
	)
)
if not defined vs_ver (
	echo %scriptname%.bat: Visual Studio %vs_num% is bad, exit
	exit /B -1
)

set vs_env_script=%vs_installdir%/%vs_ver%/VC/Auxiliary/Build/vcvarsall.bat
if not exist "%vs_env_script%" (
	echo %scriptname%.bat: "%vs_env_script%" not found
	exit /B -1
)

call "%vs_env_script%" %platform%
exit /B 0
