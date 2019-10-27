//
// Created by 贝克街的流浪猫 on 17/07/2018.
//

#ifndef RTC_PEERCONNECTION_H
#define RTC_PEERCONNECTION_H


#include "rtc/rtc_observer.h"
#include "rtc/datachannel/data_channel.h"
#include "media/video/fake_video_capturer.h"

class RTC;

class PeerConnection {
public:
    PeerConnection(rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection,
                   PeerConnectionObserver *peer_connection_observer, bool is_connect_to_audio_card, int max_bit_rate, std::string key);

    webrtc::SdpParseError SetLocalDescription(JNIEnv *env, jobject sdp);

    webrtc::SdpParseError SetRemoteDescription(JNIEnv *env, jobject sdp);

    void CreateOffer(jobject java_observer);

    int GetSignalingState();

    void CreateAnswer(jobject java_observer);

    void OpenAudio(RTC *rtc);

    void CloseAudio();

    void StartTransport(RTC *rtc);

    void StopTransport();

    webrtc::SdpParseError AddIceCandidate(std::string sdp_mid, int sdp_mline_index, std::string candidate);

    DataChannel *CreateDataChannel(std::string label, webrtc::DataChannelInit config, DataChannelObserver *observer);

    ~PeerConnection();

    void ChangeBitrate(int bitrate);

private:
    cricket::AudioOptions GetAudioOptions();

    rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection;
    PeerConnectionObserver *peer_connection_observer;
    bool is_connect_to_audio_card;
    std::string key;
    SetSessionDescriptionObserver *set_session_description_observer;
    CreateSessionDescriptionObserver *create_session_observer;
    rtc::scoped_refptr<webrtc::AudioSourceInterface> audio_source;
    rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track;
    rtc::scoped_refptr<webrtc::MediaStreamInterface> audio_stream;
    rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> video_source;
    rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track;
    rtc::scoped_refptr<webrtc::MediaStreamInterface> transport_stream;
};

#endif //RTC_PEERCONNECTION_H
