#!/bin/bash --login
path=`dirname "$0"`
cd "$path"

target="$1"

rm -rf build
../gradle/bin/gradle jarRelease

# if [ -n "$target" ] && [ "$target" = "msdk" ]; then
# ../gradle/bin/gradle -b build_msdk.gradle jarRelease
# fi

# if [ -z "$target" ] || [ "$target" = "nonemsdk" ]; then
# ../gradle/bin/gradle jarRelease
# fi

#cp build/libs/jdkplugin.jar ../../../../Plugins/Android/jdk-native.jar
