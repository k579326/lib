#!/bin/sh


mkdir macbuild
cd macbuild

basepath=$(cd `dirname $0`; pwd)
echo ${basepath}

cmake -DCMAKE_BUILD_TYPE=Release -DDOE_RELEASE_FLAG=ON -DDOE_ENV_RUNNING=1 ../.. -DINSTALL_PATH=${basepath}/../OUTPUT/

make






