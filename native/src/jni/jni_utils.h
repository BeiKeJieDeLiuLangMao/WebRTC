//
// Created by 贝克街的流浪猫 on 18/07/2018.
//

#ifndef RTC_JNIUTILS_H
#define RTC_JNIUTILS_H

#include <jni.h>
#include <functional>
#include <string>
#include <api/datachannelinterface.h>
#include <api/jsep.h>
#include <api/video/video_frame.h>
#include <api/peerconnectioninterface.h>

JNIEnv *ATTACH_CURRENT_THREAD_IF_NEEDED();

void DETACH_CURRENT_THREAD_IF_NEEDED();

std::string J_STRING_2_STRING(JNIEnv *env, jstring string);

jstring STRING_2_J_STRING(JNIEnv *env, std::string string);

jobject INT_2_J_INTEGER(JNIEnv *env, int number);

webrtc::DataChannelInit PARSE_DATA_CHANNEL_INIT(JNIEnv *env, jobject java_object);

webrtc::PeerConnectionInterface::RTCConfiguration GENERATE_CONFIG_FROM_J_TURN_ARRAY(jobjectArray turns);

std::string GET_STRING_FROM_OBJECT(JNIEnv *env, jobject java_object, char *field);

jbyteArray CHAR_POINTER_2_J_BYTE_ARRAY(JNIEnv *env, const unsigned char *bytes, int byte_size);

jclass GET_CANDIDATE_CLASS();

jclass GET_DATA_CHANNEL_CLASS();

jclass GET_DATA_BUFFER_CLASS();

jclass GET_SESSION_DESCRIPTION_CLASS();

jmethodID GET_VIDEO_FRAME_ROTATION_GETTER_METHOD();

jmethodID GET_VIDEO_FRAME_TIMESTAMP_GETTER_METHOD();

jmethodID GET_VIDEO_FRAME_BUFFER_GETTER_METHOD();

jmethodID GET_VIDEO_FRAME_LENGTH_GETTER_METHOD();

unsigned char *J_BYTE_ARRAY_2_CHAR_POINTER(JNIEnv *env, jbyteArray);

enum LogLevel {
    INFO = 0,
    DEBUG = 1,
    WARNING = 2,
    ERROR = 3,
};

void WEBRTC_LOG(std::string content, LogLevel level);

JavaVM *GET_GLOBAL_JAVA_VM();

#endif //RTC_JNIUTILS_H
