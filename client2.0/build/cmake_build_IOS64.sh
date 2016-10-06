#!/usr/bin/env bash
this_dir=`pwd`

export PATH=$this_dir/cmake/mac/bin/CMake.app/Contents/bin:$PATH
echo $PATH
cd ..
mkdir IOS
cd IOS

cmake -D__IOS__=1 -DCMAKE_TOOLCHAIN_FILE=./build/toolchain/ios.cmake -GXcode ..

