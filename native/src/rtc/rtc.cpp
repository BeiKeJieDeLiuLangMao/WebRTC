#include <climits>
#include <utility>// Created by 贝克街的流浪猫 on 16/07/2018.

#include "rtc/rtc.h"
#include "jni/bbm_webrtc_rtc4j_core_RTC.h"
#include "rtc/rtc_observer.h"
#include "media/codec/video_encoder_decoder_factory.h"
#include "iostream"
#include "rtc/peerconnection/peer_connection.h"
#include "media/audio/audio_device_module_wrapper.h"
#include "rtc/network/socket_factory_wrapper.h"
#include <api/peerconnectioninterface.h>
#include <p2p/client/basicportallocator.h>
#include <p2p/base/packetsocketfactory.h>
#include <rtc_base/thread.h>
#include <rtc_base/ssladapter.h>
#include "rtc_base/bind.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"

void OnDataChannelCreated(jobject java_observer, rtc::scoped_refptr<webrtc::DataChannelInterface> channel);

RTC::RTC(std::string white_ip_prefix, int min_port, int max_port, std::string key) {
    this->min_port = min_port;
    this->max_port = max_port;
    this->white_private_ip_prefix = std::move(white_ip_prefix);
    this->key = std::move(key);
}

void RTC::Init(jobject audio_capturer) {
    InitThreads();
    audio_device_module = worker_thread->Invoke<rtc::scoped_refptr<webrtc::AudioDeviceModule>>(
            RTC_FROM_HERE,
            rtc::Bind(
                    &RTC::InitJavaAudioDeviceModule,
                    this,
                    audio_capturer));
    WEBRTC_LOG(key + ": After fake audio device module.", INFO);
    InitFactory();
}

void RTC::Init(jobject audio_capturer, jobject video_capturer) {
    this->video_capturer = video_capturer;
    InitThreads();
    audio_device_module = worker_thread->Invoke<rtc::scoped_refptr<webrtc::AudioDeviceModule>>(
            RTC_FROM_HERE,
            rtc::Bind(
                    &RTC::InitJavaAudioDeviceModule,
                    this,
                    audio_capturer));
    WEBRTC_LOG(key + ": After fake audio device module.", INFO);
    InitFactory();
}

void RTC::Init(std::string audio_device_match_string) {
    InitThreads();
    audio_device_module = worker_thread->Invoke<rtc::scoped_refptr<webrtc::AudioDeviceModule>>(
            RTC_FROM_HERE,
            rtc::Bind(
                    &RTC::InitPlatformAudioDeviceModule,
                    this, std::move(
                            audio_device_match_string)));
    InitFactory();
}

void RTC::OpenHardwareAccelerate() {
    this->hardware_accelerate = true;
}

void RTC::InitThreads() {
    signaling_thread = rtc::Thread::Create();
    signaling_thread->SetName("signaling", nullptr);
    RTC_CHECK(signaling_thread->Start()) << "Failed to start thread";
    WEBRTC_LOG(key + ": Original socket server used.", INFO);
    worker_thread = rtc::Thread::Create();
    worker_thread->SetName("worker", nullptr);
    RTC_CHECK(worker_thread->Start()) << "Failed to start thread";
    network_thread = rtc::Thread::CreateWithSocketServer();
    network_thread->SetName("network", nullptr);
    RTC_CHECK(network_thread->Start()) << "Failed to start thread";
}


void RTC::InitFactory() {
    socket_factory.reset(
            new rtc::SocketFactoryWrapper(network_thread.get(), this->white_private_ip_prefix, this->min_port,
                                          this->max_port, this->key));
    network_manager.reset(new rtc::BasicNetworkManager());
    peer_connection_factory = webrtc::CreatePeerConnectionFactory(
            network_thread.get(), worker_thread.get(), signaling_thread.get(), audio_device_module,
            webrtc::CreateBuiltinAudioEncoderFactory(), webrtc::CreateBuiltinAudioDecoderFactory(),
            CreateVideoEncoderFactory(hardware_accelerate, key), CreateVideoDecoderFactory(key),
            nullptr, nullptr);
}

PeerConnection *
RTC::CreatePeerConnection(PeerConnectionObserver *peerConnectionObserver, jobjectArray turns, int max_bit_rate) {
    webrtc::PeerConnectionInterface::RTCConfiguration configuration = GENERATE_CONFIG_FROM_J_TURN_ARRAY(turns);
    configuration.tcp_candidate_policy = webrtc::PeerConnectionInterface::TcpCandidatePolicy::kTcpCandidatePolicyDisabled;
    configuration.audio_jitter_buffer_fast_accelerate = true;
    std::unique_ptr<cricket::PortAllocator> port_allocator(
            new cricket::BasicPortAllocator(network_manager.get(), socket_factory.get()));
    port_allocator->SetPortRange(this->min_port, this->max_port);
    return new PeerConnection(peer_connection_factory->CreatePeerConnection(
            configuration, std::move(port_allocator), nullptr, peerConnectionObserver), peerConnectionObserver,
                              is_connect_to_audio_card, max_bit_rate, this->key);
}

rtc::scoped_refptr<webrtc::MediaStreamInterface> RTC::CreateLocalMediaStream(const std::string &label) {
    return peer_connection_factory->CreateLocalMediaStream(label);
}

rtc::scoped_refptr<webrtc::AudioSourceInterface> RTC::CreateAudioSource(const cricket::AudioOptions &options) {
    return peer_connection_factory->CreateAudioSource(options);
}

rtc::scoped_refptr<webrtc::AudioTrackInterface> RTC::CreateAudioTrack(const std::string &label,
                                                                      webrtc::AudioSourceInterface *source) {
    return peer_connection_factory->CreateAudioTrack(label, source);
}

rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> RTC::CreateVideoSource(cricket::VideoCapturer *capturer) {
    return peer_connection_factory->CreateVideoSource(capturer);
}

rtc::scoped_refptr<webrtc::VideoTrackInterface> RTC::CreateVideoTrack(const std::string &label,
                                                                      webrtc::VideoTrackSourceInterface *source) {
    return peer_connection_factory->CreateVideoTrack(label, source);
}

rtc::scoped_refptr<webrtc::AudioDeviceModule> RTC::InitJavaAudioDeviceModule(jobject audio_capturer) {
    RTC_DCHECK(worker_thread.get() == rtc::Thread::Current());
    WEBRTC_LOG(key + ": Create fake audio device module.", INFO);
    auto result = new rtc::RefCountedObject<FakeAudioDeviceModule>(
            FakeAudioDeviceModule::CreateJavaCapturerWrapper(audio_capturer, key),
            FakeAudioDeviceModule::CreateDiscardRenderer(44100), 1, key);
    WEBRTC_LOG(key + ": Create fake audio device module finished.", INFO);
    is_connect_to_audio_card = true;
    return result;
}

rtc::scoped_refptr<webrtc::AudioDeviceModule>
RTC::InitPlatformAudioDeviceModule(std::string audio_device_match_string) {
    RTC_DCHECK(worker_thread.get() == rtc::Thread::Current());
    auto *audio_device = new rtc::RefCountedObject<audio_device_module_wrapper>(
            webrtc::AudioDeviceModule::AudioLayer::kLinuxPulseAudio);
    audio_device->Init();
    int16_t num = audio_device->RecordingDevices();
    for (int16_t i = 0; i < num; i++) {
        char name[webrtc::kAdmMaxDeviceNameSize];
        char guid[webrtc::kAdmMaxGuidSize];
        audio_device->RecordingDeviceName(static_cast<uint16_t>(i), name, guid);
        std::string name_str(name);
        WEBRTC_LOG(key + ": Audio device name: " + name_str, INFO);
        if (!audio_device_match_string.empty() && name_str.find(audio_device_match_string) == 0) {
            audio_device->SetDeviceId(static_cast<uint16_t>(i));
        }
    }
    is_connect_to_audio_card = true;
    return audio_device;
}

void RTC::ReleaseAudioDeviceModule() {
    RTC_DCHECK(worker_thread.get() == rtc::Thread::Current());
    audio_device_module = nullptr;
}

void RTC::DetachCurrentThread() {
    DETACH_CURRENT_THREAD_IF_NEEDED();
}

RTC::~RTC() {
    peer_connection_factory = nullptr;
    WEBRTC_LOG(key + ": Destroy peer connection factory", INFO);
    worker_thread->Invoke<void>(RTC_FROM_HERE, rtc::Bind(&RTC::ReleaseAudioDeviceModule, this));
    signaling_thread->Invoke<void>(RTC_FROM_HERE, rtc::Bind(&RTC::DetachCurrentThread, this));
    worker_thread->Invoke<void>(RTC_FROM_HERE, rtc::Bind(&RTC::DetachCurrentThread, this));
    network_thread->Invoke<void>(RTC_FROM_HERE, rtc::Bind(&RTC::DetachCurrentThread, this));
    worker_thread->Stop();
    signaling_thread->Stop();
    network_thread->Stop();
    worker_thread.reset();
    signaling_thread.reset();
    network_thread.reset();
    network_manager = nullptr;
    socket_factory = nullptr;
    WEBRTC_LOG(key + ": Stop threads", INFO);
    if (video_capturer) {
        JNIEnv *env = ATTACH_CURRENT_THREAD_IF_NEEDED();
        env->DeleteGlobalRef(video_capturer);
    }
}

FakeVideoCapturer *RTC::CreateFakeVideoCapturerInSignalingThread() {
    if (video_capturer) {
        return signaling_thread->Invoke<FakeVideoCapturer *>(RTC_FROM_HERE,
                                                             rtc::Bind(&RTC::CreateFakeVideoCapturer, this,
                                                                       video_capturer));
    } else {
        return nullptr;
    }
}

FakeVideoCapturer *RTC::CreateFakeVideoCapturer(jobject java_object) {
    return new FakeVideoCapturer(java_object, this->key);
}

//Write observer handler here to avoid loop dependency
void OnDataChannelCreated(jobject java_observer, rtc::scoped_refptr<webrtc::DataChannelInterface> channel) {
    DataChannel *data_channel = new DataChannel(std::move(channel));
    JNIEnv *env = ATTACH_CURRENT_THREAD_IF_NEEDED();
    jclass data_channel_class = GET_DATA_CHANNEL_CLASS();
    jmethodID init_method = env->GetMethodID(data_channel_class, "<init>", "(J)V");
    jobject java_data_channel = env->NewObject(data_channel_class, init_method, (jlong) data_channel);
    jclass observer_class = env->GetObjectClass(java_observer);
    jmethodID java_event_method = env->GetMethodID(observer_class, "onDataChannel",
                                                   "(Lbbm/webrtc/rtc4j/core/DataChannel;)V");
    env->CallVoidMethod(java_observer, java_event_method, java_data_channel);
    env->DeleteLocalRef(java_data_channel);
    env->DeleteLocalRef(observer_class);
}

JNIEXPORT jlong JNICALL
Java_bbm_webrtc_rtc4j_core_RTC_createNativeObject__Lbbm_webrtc_rtc4j_core_audio_AudioCapturer_2Ljava_lang_String_2IILjava_lang_String_2
        (JNIEnv *env, jobject, jobject audio_capturer, jstring white_private_ip_prefix, jint min_port, jint max_port, jstring key) {
    auto rtc = new RTC(J_STRING_2_STRING(env, white_private_ip_prefix), min_port, max_port, J_STRING_2_STRING(env, key));
    rtc->Init(env->NewGlobalRef(audio_capturer));
    return (jlong) rtc;
};

JNIEXPORT jlong JNICALL
Java_bbm_webrtc_rtc4j_core_RTC_createNativeObject__Lbbm_webrtc_rtc4j_core_audio_AudioCapturer_2Lbbm_webrtc_rtc4j_core_video_VideoCapturer_2Ljava_lang_String_2IIZLjava_lang_String_2
        (JNIEnv *env, jobject, jobject audio_capturer, jobject video_capturer, jstring white_private_ip_prefix,
         jint min_port, jint max_port, jboolean hardware_accelerate, jstring key) {
    auto rtc = new RTC(J_STRING_2_STRING(env, white_private_ip_prefix), min_port, max_port, J_STRING_2_STRING(env, key));
    if (hardware_accelerate) {
        rtc->OpenHardwareAccelerate();
    }
    rtc->Init(env->NewGlobalRef(audio_capturer), env->NewGlobalRef(video_capturer));
    return (jlong) rtc;
}

JNIEXPORT jlong JNICALL
Java_bbm_webrtc_rtc4j_core_RTC_createNativeObject__Ljava_lang_String_2Ljava_lang_String_2IILjava_lang_String_2
        (JNIEnv *env, jobject, jstring match_value, jstring white_private_ip_prefix, jint min_port, jint max_port, jstring key) {
    auto rtc = new RTC(J_STRING_2_STRING(env, white_private_ip_prefix), min_port, max_port, J_STRING_2_STRING(env, key));
    rtc->Init(J_STRING_2_STRING(env, match_value));
    return (jlong) rtc;;
}

JNIEXPORT jlong JNICALL Java_bbm_webrtc_rtc4j_core_RTC_createNativePeerConnection
        (JNIEnv *env, jobject, jlong thiz, jobject observer_object, jobjectArray turns, jint max_bit_rate) {
    return (jlong) ((RTC *) thiz)->CreatePeerConnection(
            new PeerConnectionObserver(env->NewGlobalRef(observer_object), OnDataChannelCreated,
                                       ((RTC *) thiz)->white_private_ip_prefix, ((RTC *) thiz)->key),
            turns,
            max_bit_rate);
};

JNIEXPORT void JNICALL Java_bbm_webrtc_rtc4j_core_RTC_free
        (JNIEnv *, jobject, jlong thiz) {
    delete (RTC *) thiz;
};





