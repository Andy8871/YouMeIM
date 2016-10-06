#!/usr/bin/env bash
this_dir=`pwd`

export PATH=$this_dir/cmake/mac/bin/CMake.app/Contents/bin:$PATH
echo $PATH
cd ..
mkdir IOS_Simulator
cd IOS_Simulator

cmake -D__IOSSIMULATOR__=1 -DIOS_PLATFORM=SIMULATOR -DCMAKE_TOOLCHAIN_FILE=./build/toolchain/ios.cmake -GXcode ..

