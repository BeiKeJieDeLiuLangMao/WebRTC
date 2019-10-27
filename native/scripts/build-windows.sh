#!/usr/bin/env bash
echo 'Generate build files...'
cmake -G "Visual Studio 14 2015 Win64" ..
echo 'Build By Visual Studio...'
echo 'Copy Begin...'
cp ./Release/rtc.dll ../../rtc4j/src/main/resources/jni/
echo 'Finished'