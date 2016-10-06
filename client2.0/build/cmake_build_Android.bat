@echo off
cd ..
mkdir Android
cd Android



cmake .. -G "NMake Makefiles" -DCMAKE_TOOLCHAIN_FILE=E:\tools\android-cmake\android.toolchain.cmake -DCMAKE_MAKE_PROGRAME=$(ANDROID_NDK)/prebuilt/windows/bin/make.exe -D__ANDROID__=1 -DCMAKE_BUILD_TYPE=Release -DANDROID_ABI="armeabi"
nmake
pause

@echo on
