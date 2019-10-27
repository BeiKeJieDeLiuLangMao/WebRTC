//
// Created by 贝克街的流浪猫 on 20/08/2018.
//

#ifndef RTC_RTC_H
#define RTC_RTC_H
#include "media/audio/fake_audio_device_module.h"
#include "rtc/peerconnection/peer_connection.h"
#include "rtc/network/socket_factory_wrapper.h"

class RTC {
public:
    RTC(std::string white_ip_prefix, int min_port, int max_port, std::string key);

    void Init(jobject audio_capturer);

    void Init(jobject audio_capturer, jobject video_capturer);

    void Init(std::string audio_device_match_string);

    void OpenHardwareAccelerate();

    PeerConnection *CreatePeerConnection(PeerConnectionObserver *peerConnectionObserver, jobjectArray turns, int max_bit_rate);

    rtc::scoped_refptr<webrtc::MediaStreamInterface> CreateLocalMediaStream(const std::string &label);

    rtc::scoped_refptr<webrtc::AudioSourceInterface> CreateAudioSource(const cricket::AudioOptions &options);

    rtc::scoped_refptr<webrtc::AudioTrackInterface> CreateAudioTrack(const std::string &label,
                                                                     webrtc::AudioSourceInterface *source);
    FakeVideoCapturer* CreateFakeVideoCapturerInSignalingThread();
    rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> CreateVideoSource(cricket::VideoCapturer* capturer);

    rtc::scoped_refptr<webrtc::VideoTrackInterface> CreateVideoTrack(const std::string &label,
                                                                          webrtc::VideoTrackSourceInterface *source);

    std::string white_private_ip_prefix;
    std::string key;
    rtc::scoped_refptr<webrtc::AudioDeviceModule> InitJavaAudioDeviceModule(jobject audio_capturer);
    rtc::scoped_refptr<webrtc::AudioDeviceModule> InitPlatformAudioDeviceModule(std::string audio_device_match_string);
    void ReleaseAudioDeviceModule();
    void DetachCurrentThread();
    ~RTC();

private:
    void InitThreads();
    FakeVideoCapturer*CreateFakeVideoCapturer(jobject video_capturer);
    void InitFactory();

    std::unique_ptr<rtc::Thread> signaling_thread;
    std::unique_ptr<rtc::Thread> worker_thread;
    std::unique_ptr<rtc::Thread> network_thread;
    rtc::scoped_refptr<webrtc::AudioDeviceModule> audio_device_module;
    std::unique_ptr<rtc::BasicNetworkManager> network_manager;
    std::unique_ptr<rtc::SocketFactoryWrapper> socket_factory;
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory;
    jobject video_capturer = nullptr;
    int min_port;
    int max_port;
    bool is_connect_to_audio_card;
    bool hardware_accelerate = false;

};

#endif //RTC_RTC_H
