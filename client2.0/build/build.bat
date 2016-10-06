@echo off
set ANDROID_NDK=E:\tools\android-ndk-r10e
cd ..
mkdir Win32
cd Win32
cmake .. -G "NMake Makefiles" -DCMAKE_TOOLCHAIN_FILE=E:\tools\android-cmake\android.toolchain.cmake -DCMAKE_MAKE_PROGRAME=$(ANDROID_NDK)/prebuilt/windows/bin/make.exe
nmake

pause
@echo on