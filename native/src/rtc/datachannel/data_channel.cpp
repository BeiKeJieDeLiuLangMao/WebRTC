//
// Created by 贝克街的流浪猫 on 18/07/2018.
//

#include "data_channel.h"
#include "jni/bbm_webrtc_rtc4j_core_DataChannel.h"
#include <utility>

DataChannel::DataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel,
                         DataChannelObserver *data_channel_observer) {
    this->data_channel = std::move(data_channel);
    this->data_channel_observer = data_channel_observer;
}

DataChannel::DataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) {
    this->data_channel = std::move(data_channel);
    this->data_channel_observer = nullptr;
}

void DataChannel::Send(webrtc::DataBuffer &data_buffer) {
    data_channel->Send(data_buffer);
}

void DataChannel::RegisterObserver(DataChannelObserver *observer) {
    data_channel->RegisterObserver(observer);
    delete data_channel_observer;
    data_channel_observer = observer;
}

std::string DataChannel::label() {
    if (data_channel) {
        return data_channel->label();
    }
    return "";
}

bool DataChannel::isOpen() {
    return data_channel->state() == webrtc::DataChannelInterface::DataState::kOpen;
}

DataChannel::~DataChannel() {
    data_channel->UnregisterObserver();
    delete data_channel_observer;
    data_channel->Close();
    data_channel = nullptr;
}

JNIEXPORT void JNICALL Java_bbm_webrtc_rtc4j_core_DataChannel_nativeSend
        (JNIEnv *env, jobject, jlong thiz, jobject data_buffer, jboolean binary) {
    auto *bytes = static_cast<unsigned char *>(env->GetDirectBufferAddress(data_buffer));
    auto length = static_cast<size_t>(env->GetDirectBufferCapacity(data_buffer));
    webrtc::DataBuffer resp(rtc::CopyOnWriteBuffer(bytes, length), binary);
    ((DataChannel *) thiz)->Send(resp);
    env->DeleteLocalRef(data_buffer);
}

JNIEXPORT void JNICALL Java_bbm_webrtc_rtc4j_core_DataChannel_free
        (JNIEnv *, jobject, jlong thiz) {
    delete (DataChannel *) thiz;
}

JNIEXPORT void JNICALL Java_bbm_webrtc_rtc4j_core_DataChannel_nativeRegisterObserver
        (JNIEnv *env, jobject, jlong thiz, jobject java_observer) {
    ((DataChannel *) thiz)->RegisterObserver(new DataChannelObserver(env->NewGlobalRef(java_observer)));
}

JNIEXPORT jstring JNICALL Java_bbm_webrtc_rtc4j_core_DataChannel_nativeGetLabel
        (JNIEnv *env, jobject, jlong thiz) {
    return STRING_2_J_STRING(env, ((DataChannel *) thiz)->label());
}

JNIEXPORT jboolean JNICALL Java_bbm_webrtc_rtc4j_core_DataChannel_nativeIsOpen
        (JNIEnv *, jobject, jlong thiz) {
    return static_cast<jboolean>(((DataChannel *) thiz)->isOpen());
}
