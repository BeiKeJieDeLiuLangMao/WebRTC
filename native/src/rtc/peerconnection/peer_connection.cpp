//
// Created by 贝克街的流浪猫 on 17/07/2018.
//

#include "jni/bbm_webrtc_rtc4j_core_PeerConnection.h"
#include "rtc/rtc_observer.h"
#include "peer_connection.h"
#include "media/video/fake_video_capturer.h"
#include <api/peerconnectioninterface.h>
#include <rtc_base/physicalsocketserver.h>
#include <rtc_base/ssladapter.h>
#include <rtc_base/thread.h>
#include <utility>
#include <iostream>
#include "rtc/rtc.h"

PeerConnection::PeerConnection(rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection,
                               PeerConnectionObserver *peer_connection_observer, bool is_connect_to_audio_card, int max_bit_rate, std::string key) {
    this->peer_connection = std::move(peer_connection);
    this->peer_connection_observer = peer_connection_observer;
    this->set_session_description_observer = new SetSessionDescriptionObserver();
    this->create_session_observer = new CreateSessionDescriptionObserver(nullptr, "");
    this->is_connect_to_audio_card = is_connect_to_audio_card;
    this->ChangeBitrate(max_bit_rate);
    this->key = std::move(key);
}

PeerConnection::~PeerConnection() {
    peer_connection->Close();
    peer_connection = nullptr;
    delete peer_connection_observer;
    delete set_session_description_observer;
    delete create_session_observer;
}

webrtc::SdpParseError PeerConnection::SetLocalDescription(JNIEnv *env, jobject sdp) {
    webrtc::SdpParseError error;
    webrtc::SessionDescriptionInterface *session_description(
            webrtc::CreateSessionDescription(GET_STRING_FROM_OBJECT(env, sdp, const_cast<char *>("type")),
                                             GET_STRING_FROM_OBJECT(env, sdp, const_cast<char *>("sdp")), &error));
    peer_connection->SetLocalDescription(set_session_description_observer, session_description);
    return error;
}

webrtc::SdpParseError PeerConnection::SetRemoteDescription(JNIEnv *env, jobject sdp) {
    webrtc::SdpParseError error;
    webrtc::SessionDescriptionInterface *session_description(
            webrtc::CreateSessionDescription(GET_STRING_FROM_OBJECT(env, sdp, const_cast<char *>("type")),
                                             GET_STRING_FROM_OBJECT(env, sdp, const_cast<char *>("sdp")), &error));
    peer_connection->SetRemoteDescription(set_session_description_observer, session_description);
    return error;
}

void PeerConnection::CreateOffer(jobject java_observer) {
    create_session_observer->SetGlobalJavaObserver(java_observer, "offer");
    auto options = webrtc::PeerConnectionInterface::RTCOfferAnswerOptions();
    options.offer_to_receive_audio = false;
    options.offer_to_receive_video = false;
    peer_connection->CreateOffer(create_session_observer, options);
}

int PeerConnection::GetSignalingState() {
    return peer_connection->signaling_state();
}

void PeerConnection::CreateAnswer(jobject java_observer) {
    create_session_observer->SetGlobalJavaObserver(java_observer, "answer");
    auto options = webrtc::PeerConnectionInterface::RTCOfferAnswerOptions();
    options.offer_to_receive_audio = false;
    options.offer_to_receive_video = false;
    peer_connection->CreateAnswer(create_session_observer, options);
}

void PeerConnection::OpenAudio(RTC *rtc) {
    if (is_connect_to_audio_card) {
        WEBRTC_LOG(key + ": Open audio", INFO);
        audio_stream = rtc->CreateLocalMediaStream("stream_audio");
        audio_source = rtc->CreateAudioSource(GetAudioOptions());
        audio_track = rtc->CreateAudioTrack("audio_track", audio_source);
        audio_stream->AddTrack(audio_track);
        peer_connection->AddStream(audio_stream);
    }
}

void PeerConnection::CloseAudio() {
    if (audio_stream.get() != nullptr) {
        peer_connection->RemoveStream(audio_stream);
        audio_stream = nullptr;
        audio_track = nullptr;
        audio_source = nullptr;
    }
}

void PeerConnection::StartTransport(RTC *rtc) {
    WEBRTC_LOG(key + ": Start transport", INFO);
    transport_stream = rtc->CreateLocalMediaStream("stream");
    video_source = rtc->CreateVideoSource(rtc->CreateFakeVideoCapturerInSignalingThread());
    video_track = rtc->CreateVideoTrack("video_track", video_source.get());
    transport_stream->AddTrack(video_track);
    audio_source = rtc->CreateAudioSource(GetAudioOptions());
    audio_track = rtc->CreateAudioTrack("audio_track", audio_source);
    transport_stream->AddTrack(audio_track);
    peer_connection->AddStream(transport_stream);
}

void PeerConnection::StopTransport() {
    WEBRTC_LOG(key + ": Stop transport", INFO);
    if (transport_stream.get() != nullptr) {
        peer_connection->RemoveStream(transport_stream);
        transport_stream = nullptr;
        video_track = nullptr;
        video_source = nullptr;
        audio_track = nullptr;
        audio_source = nullptr;
    }
}

webrtc::SdpParseError PeerConnection::AddIceCandidate(std::string sdp_mid, int sdp_mline_index, std::string candidate) {
    webrtc::SdpParseError error;
    auto candidate_object = webrtc::CreateIceCandidate(sdp_mid, sdp_mline_index, candidate, &error);
    peer_connection->AddIceCandidate(candidate_object);
    return error;
}

DataChannel *
PeerConnection::CreateDataChannel(std::string label, webrtc::DataChannelInit config, DataChannelObserver *observer) {
    rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel = peer_connection->CreateDataChannel(label, &config);
    data_channel->RegisterObserver(observer);
    return new DataChannel(data_channel, observer);
}

cricket::AudioOptions PeerConnection::GetAudioOptions() {
    cricket::AudioOptions options;
    options.audio_jitter_buffer_fast_accelerate = absl::optional<bool>(true);
    options.audio_jitter_buffer_max_packets = absl::optional<int>(10);
    options.echo_cancellation = absl::optional<bool>(false);
    options.auto_gain_control = absl::optional<bool>(false);
    options.noise_suppression = absl::optional<bool>(false);
    options.highpass_filter = absl::optional<bool>(false);
    options.stereo_swapping = absl::optional<bool>(false);
    options.typing_detection = absl::optional<bool>(false);
    options.experimental_agc = absl::optional<bool>(false);
    options.extended_filter_aec = absl::optional<bool>(false);
    options.delay_agnostic_aec = absl::optional<bool>(false);
    options.experimental_ns = absl::optional<bool>(false);
    options.residual_echo_detector = absl::optional<bool>(false);
    options.audio_network_adaptor = absl::optional<bool>(true);
    return options;
}

void PeerConnection::ChangeBitrate(int bitrate) {
    auto bit_rate_setting = webrtc::BitrateSettings();
    bit_rate_setting.min_bitrate_bps = 30000;
    bit_rate_setting.max_bitrate_bps = bitrate;
    bit_rate_setting.start_bitrate_bps = bitrate;
    this->peer_connection->SetBitrate(bit_rate_setting);
}

jstring handleSdpParseError(JNIEnv *env, webrtc::SdpParseError parse_error) {
    if (parse_error.line.empty() && parse_error.description.empty()) {
        return nullptr;
    } else {
        return STRING_2_J_STRING(env, parse_error.line + ':' + parse_error.description);
    }
}

JNIEXPORT jstring JNICALL Java_bbm_webrtc_rtc4j_core_PeerConnection_nativeAddIceCandidate
        (JNIEnv *env, jobject, jlong thiz, jstring sdp_mid, jint sdp_mline_index, jstring candidate) {
    return handleSdpParseError(env, ((PeerConnection *) thiz)->AddIceCandidate(J_STRING_2_STRING(env, sdp_mid),
                                                                               sdp_mline_index,
                                                                               J_STRING_2_STRING(env, candidate)));
}

JNIEXPORT jstring JNICALL Java_bbm_webrtc_rtc4j_core_PeerConnection_nativeSetLocalDescription
        (JNIEnv *env, jobject, jlong thiz, jobject sdp) {
    return handleSdpParseError(env, ((PeerConnection *) thiz)->SetLocalDescription(env, sdp));
}

JNIEXPORT jstring JNICALL Java_bbm_webrtc_rtc4j_core_PeerConnection_nativeSetRemoteDescription
        (JNIEnv *env, jobject, jlong thiz, jobject sdp) {
    return handleSdpParseError(env, ((PeerConnection *) thiz)->SetRemoteDescription(env, sdp));
}

JNIEXPORT jobject JNICALL Java_bbm_webrtc_rtc4j_core_PeerConnection_nativeGetSignalingState
        (JNIEnv *env, jobject, jlong thiz) {
    return INT_2_J_INTEGER(env, ((PeerConnection *) thiz)->GetSignalingState());
}

JNIEXPORT void JNICALL Java_bbm_webrtc_rtc4j_core_PeerConnection_nativeChangeBitrate
        (JNIEnv *env, jobject, jlong thiz, jint bitrate) {
    ((PeerConnection *) thiz)->ChangeBitrate((int)bitrate);
}

JNIEXPORT void JNICALL Java_bbm_webrtc_rtc4j_core_PeerConnection_nativeOpenAudio
        (JNIEnv *, jobject, jlong thiz, jlong rtc_ptr) {
    ((PeerConnection *) thiz)->OpenAudio((RTC *) rtc_ptr);
}

JNIEXPORT void JNICALL Java_bbm_webrtc_rtc4j_core_PeerConnection_nativeCloseAudio
        (JNIEnv *, jobject, jlong thiz) {
    ((PeerConnection *) thiz)->CloseAudio();
}

JNIEXPORT void JNICALL Java_bbm_webrtc_rtc4j_core_PeerConnection_nativeStartTransport
        (JNIEnv *, jobject, jlong thiz, jlong rtc_ptr) {
    ((PeerConnection *) thiz)->StartTransport((RTC *) rtc_ptr);
}

JNIEXPORT void JNICALL Java_bbm_webrtc_rtc4j_core_PeerConnection_nativeStopTransport
        (JNIEnv *, jobject, jlong thiz) {
    ((PeerConnection *) thiz)->StopTransport();
}

JNIEXPORT void JNICALL Java_bbm_webrtc_rtc4j_core_PeerConnection_nativeCreateAnswer
        (JNIEnv *env, jobject, jlong thiz, jobject java_observer) {
    ((PeerConnection *) thiz)->CreateAnswer(env->NewGlobalRef(java_observer));
}

JNIEXPORT void JNICALL Java_bbm_webrtc_rtc4j_core_PeerConnection_nativeCreateOffer
        (JNIEnv *env, jobject, jlong thiz, jobject java_observer) {
    ((PeerConnection *) thiz)->CreateOffer(env->NewGlobalRef(java_observer));
}

JNIEXPORT void JNICALL Java_bbm_webrtc_rtc4j_core_PeerConnection_free
        (JNIEnv *, jobject, jlong thiz) {
    delete (PeerConnection *) thiz;
}

JNIEXPORT jlong JNICALL Java_bbm_webrtc_rtc4j_core_PeerConnection_nativeCreateDataChannel
        (JNIEnv *env, jobject, jlong thiz, jstring label, jobject config, jobject observer) {
    return (jlong) ((PeerConnection *) thiz)->CreateDataChannel(J_STRING_2_STRING(env, label),
                                                                PARSE_DATA_CHANNEL_INIT(env, config),
                                                                new DataChannelObserver(env->NewGlobalRef(observer)));
}


