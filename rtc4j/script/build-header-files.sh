#!/usr/bin/env bash
ls -l ../target/classes/bbm/webrtc/rtc4j/core| grep ^- | awk '{print $9}' |
sed 's/.class//g'|
sed 's/^/bbm.webrtc.rtc4j.core.&/g'|
xargs javah -classpath ../target/classes -d ../../native/src/jni/

