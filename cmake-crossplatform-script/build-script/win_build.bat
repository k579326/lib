(
mkdir winbuild
pushd %cd%\winbuild
echo %cd%
cmake -G"NMake Makefiles" -DDOE_RELEASE_FLAG=ON -DDOE_ENV_RUNNING=1 ../.. -DINSTALL_PATH=%cd%/output 
nmake
)