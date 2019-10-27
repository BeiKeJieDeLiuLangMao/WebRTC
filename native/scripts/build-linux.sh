#!/usr/bin/env bash
echo 'Generate build files...'
cmake ..
echo 'Build Begin...'
cmake --build ../ --target rtc -- -j 4
echo 'Copy Begin...'
cp ../librtc.so ../../rtc4j/src/main/resources/jni/
echo 'Finished'