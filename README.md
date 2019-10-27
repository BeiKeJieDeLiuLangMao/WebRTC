# WebRTC
A java wrapper for WebRTC by JNI based on WebRTC M70, you could use this java lib to communicate with any other WebRTC client(eg: chrome browser, android, ios...), stream from other client to java side isn't supported yet, if anyone would like to finish this part, your PR will been very expected.

You could use this lib to transport video stream(include audio) to other WebRTC clients.

This project may not be designed very well, but you can use it as a reference to know how to use webrtc native lib. You can take it as a start point to build your project.
# Blog about this lib
[ZhiHu](https://zhuanlan.zhihu.com/p/86762371)

[My site](https://www.beikejiedeliulangmao.top/%E5%9C%A8Java%E4%B8%AD%E4%BD%BF%E7%94%A8WebRTC%E4%BC%A0%E8%BE%93%E8%A7%86%E9%A2%91/)

# Concept
- libwebrtc: Chromium WebRTC c++ lib
- rtc4j native lib: WebRTC java wrapper jni lib
- rtc4j: WebRTC java wrapper

# Dependencies of building native
1. build libwebrtc by [libwebrtc-m70](https://github.com/BeiKeJieDeLiuLangMao/libwebrtc-m70)
2. install libwebrtc
3. install libyuv
4. install libjpegturbo
5. install ffmpeg

# Feature
1. Build connection with other webrtc client
2. Use data channel to communicate with other client
3. Send video frame from java to other client(include audio)
4. Use ffmpeg's h264 encoder
5. Use NVIDIA device to accelerate encode, if you do have a NVIDIA device installed
6. Limit java side webrtc communicate port
7. Set java side socket ip white list
8. Change webrtc video transport bandwidth
9. Change video resolution in runtime, this lib will automatically send a new key frame to change the stream resolution

# How to run it
```bash
# Step 1: Build ./native by cmake
# These are some build scripts in ./native/scripts
# and if you are in MacOS, the native lib already built and located in ./rtc4j/src/main/resources/jni

# Step 2: maven install rtc4j
cd ./rtc4j
mvn install

# Step 3: maven run spring demo
cd ./demo/spring
mvn spring-boot:run

# Step 4: maven run front vue demo
cd ./demo/vue
npm install
npm run serve
# open http://localhost:8080/ 
```

# How to develop based on it
1. As described in `Dependencies`, you should build your own webrtc native lib, it's free to upgrade webrtc native lib if necessary
2. Add new feature in both `/cpp(JNI)` side and `rtc4j(Java)` side(If you upgrade webrtc native lib, it is very likely you need to upgrade existed JNI code too)
