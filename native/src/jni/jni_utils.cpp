#include <climits>//
// Created by 贝克街的流浪猫 on 18/07/2018.
//

#include "jni_utils.h"
#include <jni.h>
#include <iostream>
#include <utility>
#include <rtc_base/ssladapter.h>
#include <thread>
#include <api/video/i420_buffer.h>
#include <rtc_base/timeutils.h>

#if defined(OS_LINUX)
#include <sys/prctl.h>
#include <sys/resource.h>
#endif

static JavaVM *g_java_vm = nullptr;
static jclass candidate_class;
static jclass data_channel_class;
static jclass data_buffer_class;
static jclass session_description_class;
static jclass rtc_class;
static jmethodID rtc_log_method;
static jclass video_frame_class;
static jmethodID video_frame_get_rotation_method;
static jmethodID video_frame_get_timestamp_method;
static jmethodID video_frame_get_buffer_method;
static jmethodID video_frame_get_buffer_length_method;

bool getBooleanFromObject(JNIEnv *env, jobject java_object, char *field) {
    jobject boolean_object = env->GetObjectField(java_object,
                                                 env->GetFieldID(env->GetObjectClass(java_object), field,
                                                                 "Ljava/lang/Boolean;"));
    bool result = env->GetBooleanField(boolean_object,
                                       env->GetFieldID(env->GetObjectClass(boolean_object), "value", "Z"));
    env->DeleteLocalRef(boolean_object);
    return result;
}

int getIntFromObject(JNIEnv *env, jobject java_object, char *field) {
    jobject integer_object = env->GetObjectField(java_object,
                                                 env->GetFieldID(env->GetObjectClass(java_object), field,
                                                                 "Ljava/lang/Integer;"));
    int result = env->GetIntField(integer_object, env->GetFieldID(env->GetObjectClass(integer_object), "value", "I"));
    env->DeleteLocalRef(integer_object);
    return result;
}

std::string GET_STRING_FROM_OBJECT(JNIEnv *env, jobject java_object, char *field) {
    return J_STRING_2_STRING(env, (jstring) env->GetObjectField(java_object,
                                                                env->GetFieldID(env->GetObjectClass(java_object), field,
                                                                                "Ljava/lang/String;")));
}

JNIEnv *GetEnv() {
    void *env = nullptr;
    jint status = g_java_vm->GetEnv(&env, JNI_VERSION_1_8);
    RTC_CHECK(((env != nullptr) && (status == JNI_OK)) ||
              ((env == nullptr) && (status == JNI_EDETACHED)))
        << "Unexpected GetEnv return: " << status << ":" << env;
    return reinterpret_cast<JNIEnv *>(env);
}

static std::string GetThreadName() {
    return "JNI-RTC";
}

JNIEnv *ATTACH_CURRENT_THREAD_IF_NEEDED() {
    JNIEnv *jni = GetEnv();
    if (jni)
        return jni;
    JavaVMAttachArgs args;
    args.version = JNI_VERSION_1_8;
    args.group = nullptr;
    args.name = const_cast<char *>("JNI-RTC");
// Deal with difference in signatures between Oracle's jni.h and Android's.
#ifdef _JAVASOFT_JNI_H_  // Oracle's jni.h violates the JNI spec!
    void *env = nullptr;
#else
    JNIEnv* env = nullptr;
#endif
    RTC_CHECK(!g_java_vm->AttachCurrentThread(&env, &args)) << "Failed to attach thread";
    RTC_CHECK(env) << "AttachCurrentThread handed back NULL!";
    jni = reinterpret_cast<JNIEnv *>(env);
    return jni;
}

void DETACH_CURRENT_THREAD_IF_NEEDED() {
    // This function only runs on threads where |g_jni_ptr| is non-NULL, meaning
    // we were responsible for originally attaching the thread, so are responsible
    // for detaching it now.  However, because some JVM implementations (notably
    // Oracle's http://goo.gl/eHApYT) also use the pthread_key_create mechanism,
    // the JVMs accounting info for this thread may already be wiped out by the
    // time this is called. Thus it may appear we are already detached even though
    // it was our responsibility to detach!  Oh well.
    if (!GetEnv())
        return;
    jint status = g_java_vm->DetachCurrentThread();
    RTC_CHECK(status == JNI_OK) << "Failed to detach thread: " << status;
    RTC_CHECK(!GetEnv()) << "Detaching was a successful no-op???";
}

std::string J_STRING_2_STRING(JNIEnv *env, jstring j_string) {
    if (!j_string)
        return "";
    jclass string_class = env->GetObjectClass(j_string);
    jmethodID get_bytes = env->GetMethodID(string_class, "getBytes", "(Ljava/lang/String;)[B");
    auto string_jbytes = (jbyteArray) env->CallObjectMethod(j_string, get_bytes, env->NewStringUTF("UTF-8"));
    auto length = (size_t) env->GetArrayLength(string_jbytes);
    jbyte *p_bytes = env->GetByteArrayElements(string_jbytes, nullptr);
    std::string ret = std::string((char *) p_bytes, length);
    env->ReleaseByteArrayElements(string_jbytes, p_bytes, JNI_ABORT);
    env->DeleteLocalRef(string_jbytes);
    env->DeleteLocalRef(string_class);
    return ret;
}

jstring STRING_2_J_STRING(JNIEnv *env, std::string string) {
    return env->NewStringUTF(string.c_str());
}

jobject INT_2_J_INTEGER(JNIEnv *env, int number) {
    jclass integer_class = env->FindClass("Ljava/lang/Integer;");
    jmethodID integer_init = env->GetMethodID(integer_class, "<init>", "(I)V");
    jobject result = env->NewObject(integer_class, integer_init, number);
    env->DeleteLocalRef(integer_class);
    return result;
}

webrtc::DataChannelInit PARSE_DATA_CHANNEL_INIT(JNIEnv *env, jobject java_object) {
    webrtc::DataChannelInit data_channel_config;
    data_channel_config.reliable = getBooleanFromObject(env, java_object, const_cast<char *>("reliable"));
    data_channel_config.ordered = getBooleanFromObject(env, java_object, const_cast<char *>("ordered"));
    data_channel_config.maxRetransmitTime = getIntFromObject(env, java_object, const_cast<char *>("maxRetransmitTime"));
    data_channel_config.maxRetransmits = getIntFromObject(env, java_object, const_cast<char *>("maxRetransmits"));
    data_channel_config.protocol = GET_STRING_FROM_OBJECT(env, java_object, const_cast<char *>("protocol"));
    data_channel_config.negotiated = getBooleanFromObject(env, java_object, const_cast<char *>("negotiated"));
    data_channel_config.id = getIntFromObject(env, java_object, const_cast<char *>("id"));
    return data_channel_config;
}

webrtc::PeerConnectionInterface::RTCConfiguration GENERATE_CONFIG_FROM_J_TURN_ARRAY(jobjectArray turns) {
    JNIEnv *env = ATTACH_CURRENT_THREAD_IF_NEEDED();
    webrtc::PeerConnectionInterface::RTCConfiguration configuration;
    jsize turn_size = env->GetArrayLength(turns);
    for (int i = 0; i < turn_size; i++) {
        jobject turn = env->GetObjectArrayElement(turns, i);
        webrtc::PeerConnectionInterface::IceServer ice_server;
        ice_server.uri = std::move(GET_STRING_FROM_OBJECT(env, turn, const_cast<char *>("uri")));
        ice_server.username = std::move(GET_STRING_FROM_OBJECT(env, turn, const_cast<char *>("username")));
        ice_server.password = std::move(GET_STRING_FROM_OBJECT(env, turn, const_cast<char *>("password")));
        std::cout <<  ice_server.uri << ":" << ice_server.username  << ":" << ice_server.password << std::endl;
        configuration.servers.push_back(ice_server);
    }
    return configuration;
}

jbyteArray CHAR_POINTER_2_J_BYTE_ARRAY(JNIEnv *env, const unsigned char *bytes, int byte_size) {
    auto *jb = (jbyte *) bytes;
    jbyteArray jbyte_array = env->NewByteArray(byte_size);
    env->SetByteArrayRegion(jbyte_array, 0, byte_size, jb);
    return jbyte_array;
}

unsigned char *J_BYTE_ARRAY_2_CHAR_POINTER(JNIEnv *env, jbyteArray jbyte_array) {
    auto byte_size = (int) env->GetArrayLength(jbyte_array);
    auto *data = new unsigned char[byte_size + 1];
    env->GetByteArrayRegion(jbyte_array, 0, byte_size, reinterpret_cast<jbyte *>(data));
    data[byte_size] = '\0';
    return data;
}

jclass GET_CANDIDATE_CLASS() {
    return candidate_class;
}

jclass GET_DATA_CHANNEL_CLASS() {
    return data_channel_class;
}

jclass GET_DATA_BUFFER_CLASS() {
    return data_buffer_class;
}

jclass GET_SESSION_DESCRIPTION_CLASS() {
    return session_description_class;
}

jmethodID GET_VIDEO_FRAME_ROTATION_GETTER_METHOD() {
    return video_frame_get_rotation_method;
}

jmethodID GET_VIDEO_FRAME_TIMESTAMP_GETTER_METHOD() {
    return video_frame_get_timestamp_method;
}

jmethodID GET_VIDEO_FRAME_BUFFER_GETTER_METHOD() {
    return video_frame_get_buffer_method;
}

jmethodID GET_VIDEO_FRAME_LENGTH_GETTER_METHOD() {
    return video_frame_get_buffer_length_method;
}

void WEBRTC_LOG(std::string content, LogLevel level) {
    JNIEnv *env;
    if (!g_java_vm) {
        std::cout << content << std::endl;
        return;
    }
    g_java_vm->GetEnv((void **) &env, JNI_VERSION_1_8);
    if (env != nullptr) {
        env->CallStaticVoidMethod(rtc_class, rtc_log_method, STRING_2_J_STRING(env, std::move(content)), level);
    } else {
        if (g_java_vm->AttachCurrentThread((void **) &env, nullptr) == JNI_OK) {
            env->CallStaticVoidMethod(rtc_class, rtc_log_method, STRING_2_J_STRING(env, std::move(content)), level);
            g_java_vm->DetachCurrentThread();
        } else {
            std::cout << "Attach jvm thread failed." << std::endl;
        }
    }
}

JavaVM *GET_GLOBAL_JAVA_VM() {
    return g_java_vm;
}

// JNI OnLoad
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *) {
    g_java_vm = vm;
    JNIEnv *env;
    g_java_vm->GetEnv((void **) &env, JNI_VERSION_1_8);
    candidate_class = (jclass) env->NewGlobalRef(env->FindClass("bbm/webrtc/rtc4j/model/IceCandidate"));
    data_channel_class = (jclass) env->NewGlobalRef(env->FindClass("bbm/webrtc/rtc4j/core/DataChannel"));
    data_buffer_class = (jclass) env->NewGlobalRef(env->FindClass("bbm/webrtc/rtc4j/model/DataBuffer"));
    session_description_class = (jclass) env->NewGlobalRef(
            env->FindClass("bbm/webrtc/rtc4j/model/SessionDescription"));
    rtc_class = (jclass) env->NewGlobalRef(env->FindClass("bbm/webrtc/rtc4j/core/RTC"));
    rtc_log_method = env->GetStaticMethodID(rtc_class, "log", "(Ljava/lang/String;I)V");
    video_frame_class = (jclass) env->NewGlobalRef(env->FindClass("bbm/webrtc/rtc4j/model/VideoFrame"));
    video_frame_get_rotation_method = env->GetMethodID(video_frame_class, "getRotation", "()I");
    video_frame_get_timestamp_method = env->GetMethodID(video_frame_class, "getTimestamp", "()J");
    video_frame_get_buffer_method = env->GetMethodID(video_frame_class, "getDataBuffer", "()Ljava/nio/ByteBuffer;");
    video_frame_get_buffer_length_method =  env->GetMethodID(video_frame_class, "getLength", "()I");
    RTC_CHECK(rtc::InitializeSSL()) << "Failed to InitializeSSL()";
    return JNI_VERSION_1_8;
}

JNIEXPORT void JNICALL
JNI_OnUnload(JavaVM *vm, void *reserved) {
    RTC_CHECK(rtc::CleanupSSL()) << "Failed to CleanupSSL()";
}

