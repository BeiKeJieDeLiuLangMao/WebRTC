//
// Created by 贝克街的流浪猫 on 18/07/2018.
//

#ifndef WEBRTC_EXAMPLE_SERVER_OBSERVERS_H
#define WEBRTC_EXAMPLE_SERVER_OBSERVERS_H

#include <api/peerconnectioninterface.h>
#include <iostream>
#include <functional>
#include <utility>
#include <jni.h>
#include "jni/jni_utils.h"

// PeerConnection events.
class PeerConnectionObserver : public webrtc::PeerConnectionObserver {
public:
    // Constructor taking a few callbacks.
    PeerConnectionObserver(jobject java_observer,
                           std::function<void(jobject,
                                              rtc::scoped_refptr<webrtc::DataChannelInterface>)> on_data_channel,
                           std::string white_private_ip_prefix,
                           std::string key) {
        this->java_observer = java_observer;
        this->on_data_channel = std::move(on_data_channel);
        this->white_private_ip_prefix = std::move(white_private_ip_prefix);
        this->key = std::move(key);
    }

    ~PeerConnectionObserver() {
        JNIEnv *env = ATTACH_CURRENT_THREAD_IF_NEEDED();
        env->DeleteGlobalRef(java_observer);
        java_observer = nullptr;
    }

    // Override signaling change.
    void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override {
        JNIEnv *env = ATTACH_CURRENT_THREAD_IF_NEEDED();
        jclass observer_class = env->GetObjectClass(java_observer);
        jmethodID java_event_method = env->GetMethodID(observer_class, "onSignalingChange", "(I)V");
        env->CallVoidMethod(java_observer, java_event_method, (int) new_state);
        env->DeleteLocalRef(observer_class);
    }

    // Override adding a stream.
    void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override {

    }

    // Override removing a stream.
    void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override {}

    // Override data channel change.
    void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> channel) override {
        on_data_channel(java_observer, channel);
    }

    // Override renegotiation.
    void OnRenegotiationNeeded() override {
        JNIEnv *env = ATTACH_CURRENT_THREAD_IF_NEEDED();
        jclass observer_class = env->GetObjectClass(java_observer);
        jmethodID java_event_method = env->GetMethodID(observer_class, "onRenegotiationNeeded", "()V");
        env->CallVoidMethod(java_observer, java_event_method);
        env->DeleteLocalRef(observer_class);
    }

    // Override ICE connection change.
    void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override {
    }

    // Override ICE gathering change.
    void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override {

    }

    // Override ICE candidate.
    void OnIceCandidate(const webrtc::IceCandidateInterface *candidate) override {
        WEBRTC_LOG(key + ": OnIceCandidate:" + candidate->candidate().address().ToString(), INFO);
        if (!candidate->candidate().address().IsPrivateIP() ||
            candidate->candidate().address().HostAsURIString().find(this->white_private_ip_prefix) == 0) {
            JNIEnv *env = ATTACH_CURRENT_THREAD_IF_NEEDED();
            jclass candidate_class = GET_CANDIDATE_CLASS();
            jmethodID init_method = env->GetMethodID(candidate_class, "<init>",
                                                     "(Ljava/lang/String;Ljava/lang/Integer;Ljava/lang/String;)V");
            std::string candidate_str;
            candidate->ToString(&candidate_str);
            jobject candidate_object = env->NewObject(candidate_class, init_method,
                                                      STRING_2_J_STRING(env, candidate->sdp_mid()),
                                                      INT_2_J_INTEGER(env, candidate->sdp_mline_index()),
                                                      STRING_2_J_STRING(env, candidate_str));
            jclass observer_class = env->GetObjectClass(java_observer);
            jmethodID java_event_method = env->GetMethodID(observer_class, "onIceCandidate",
                                                           "(Lbbm/webrtc/rtc4j/model/IceCandidate;)V");
            env->CallVoidMethod(java_observer, java_event_method, candidate_object);
            env->DeleteLocalRef(candidate_object);
            env->DeleteLocalRef(observer_class);
        } else {
            WEBRTC_LOG(key + ": On ice candidate event cancelled, this ip is not in white list:" +
                       candidate->candidate().address().HostAsURIString(), INFO);
        }

    }

private:
    jobject java_observer;
    std::function<void(jobject, rtc::scoped_refptr<webrtc::DataChannelInterface>)> on_data_channel;
    std::string white_private_ip_prefix;
    std::string key;
};

// DataChannel events.
class DataChannelObserver : public webrtc::DataChannelObserver {
public:
    // Constructor taking a callback.
    DataChannelObserver(jobject java_observer) {
        this->java_observer = java_observer;
    }

    ~DataChannelObserver() {
        JNIEnv *env = ATTACH_CURRENT_THREAD_IF_NEEDED();
        env->DeleteGlobalRef(java_observer);
        java_observer = nullptr;
    }

    // Change in state of the Data Channel.
    void OnStateChange() override {}

    // Message received.
    void OnMessage(const webrtc::DataBuffer &buffer) override {
        // on_message(buffer);
        JNIEnv *env = ATTACH_CURRENT_THREAD_IF_NEEDED();
        jbyteArray jbyte_array = CHAR_POINTER_2_J_BYTE_ARRAY(env, buffer.data.cdata(),
                                                             static_cast<int>(buffer.data.size()));
        jclass data_buffer = GET_DATA_BUFFER_CLASS();
        jmethodID init_method = env->GetMethodID(data_buffer, "<init>", "([BZ)V");
        jobject data_buffer_object = env->NewObject(data_buffer, init_method,
                                                    jbyte_array,
                                                    buffer.binary);
        jclass observer_class = env->GetObjectClass(java_observer);
        jmethodID java_event_method = env->GetMethodID(observer_class, "onMessage",
                                                       "(Lbbm/webrtc/rtc4j/model/DataBuffer;)V");
        env->CallVoidMethod(java_observer, java_event_method, data_buffer_object);
        env->ReleaseByteArrayElements(jbyte_array, env->GetByteArrayElements(jbyte_array, nullptr), JNI_ABORT);
        env->DeleteLocalRef(data_buffer_object);
        env->DeleteLocalRef(observer_class);
    }

    // Buffered amount change.
    void OnBufferedAmountChange(uint64_t previous_amount) override {

    }

private:
    jobject java_observer;
};

// Set SessionDescription events.
class SetSessionDescriptionObserver : public webrtc::SetSessionDescriptionObserver {
public:
    // Default constructor.
    SetSessionDescriptionObserver() {}

    // Successfully set a session description.
    void OnSuccess() override {

    }

    // Failure to set a sesion description.
    void OnFailure(const std::string &error) override {

    }

    // Unimplemented virtual function.
    void AddRef() const override { return; }

    // Unimplemented virtual function.
    rtc::RefCountReleaseStatus Release() const override { return rtc::RefCountReleaseStatus::kDroppedLastRef; }
};

// Create SessionDescription events.
class CreateSessionDescriptionObserver : public webrtc::CreateSessionDescriptionObserver {
public:
    // Constructor taking a callback.
    CreateSessionDescriptionObserver(jobject java_observer, std::string type) {
        this->java_observer = java_observer;
        this->type = std::move(type);
    }

    ~CreateSessionDescriptionObserver() {
        JNIEnv *env = ATTACH_CURRENT_THREAD_IF_NEEDED();
        if (this->java_observer) {
            env->DeleteGlobalRef(java_observer);
            java_observer = nullptr;
        }
    }

    void SetGlobalJavaObserver(jobject java_observer, std::string type) {
        if (this->java_observer) {
            JNIEnv *env = ATTACH_CURRENT_THREAD_IF_NEEDED();
            env->DeleteGlobalRef(this->java_observer);
            this->java_observer = nullptr;
        }
        this->java_observer = java_observer;
        this->type = std::move(type);
    }

    // Successfully created a session description.
    void OnSuccess(webrtc::SessionDescriptionInterface *desc) override {
        std::string sdp;
        desc->ToString(&sdp);
        JNIEnv *env = ATTACH_CURRENT_THREAD_IF_NEEDED();
        jclass session_description = GET_SESSION_DESCRIPTION_CLASS();
        jmethodID init_method = env->GetMethodID(session_description, "<init>",
                                                 "(Ljava/lang/String;Ljava/lang/String;)V");
        jobject session_description_object = env->NewObject(session_description, init_method,
                                                            STRING_2_J_STRING(env, type),
                                                            STRING_2_J_STRING(env, sdp));
        jclass observer_class = env->GetObjectClass(java_observer);
        jmethodID java_event_method = env->GetMethodID(observer_class, "OnSuccess",
                                                       "(Lbbm/webrtc/rtc4j/model/SessionDescription;)V");
        env->CallVoidMethod(java_observer, java_event_method, session_description_object);
        env->DeleteLocalRef(session_description_object);
        env->DeleteLocalRef(observer_class);
    }

    // Failure to create a session description.
    void OnFailure(const std::string &error) override {

    }

    // Unimplemented virtual function.
    void AddRef() const override { return; }

    // Unimplemented virtual function.
    rtc::RefCountReleaseStatus Release() const override { return rtc::RefCountReleaseStatus::kDroppedLastRef; }

private:
    jobject java_observer;
    std::string type;
};

#endif  // WEBRTC_EXAMPLE_SERVER_OBSERVERS_H
