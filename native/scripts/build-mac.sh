#!/usr/bin/env bash
echo 'Build Begin...'
cmake --build ../cmake-build --target rtc -- -j 4
echo 'Copy Begin...'
cp ../cmake-build/librtc.dylib ../../rtc4j/src/main/resources/jni/
echo 'Finished'