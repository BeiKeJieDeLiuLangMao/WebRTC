//
// Created by 贝克街的流浪猫 on 14/08/2018.
//

#include <modules/audio_device/include/audio_device.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <jni.h>
#include <jni/jni_utils.h>

#include "rtc_base/buffer.h"
#include "rtc_base/criticalsection.h"
#include "rtc_base/event.h"
#include "rtc_base/platform_thread.h"
#include <mutex>
#include <condition_variable>


namespace webrtc {
    class EventTimerWrapper;
}
class FakeAudioDeviceModule : public webrtc::AudioDeviceModule {
public:
    // Returns the number of samples that Capturers and Renderers with this
    // sampling frequency will work with every time Capture or Render is called.
    static size_t SamplesPerFrame(int sampling_frequency_in_hz);

    class Capturer {
    public:
        virtual bool isJavaWrapper() {
            return false;
        }

        virtual ~Capturer() {}

        // Returns the sampling frequency in Hz of the audio data that this
        // capturer produces.
        virtual int SamplingFrequency() = 0;

        // Replaces the contents of |buffer| with 10ms of captured audio data
        // (see FakeAudioDevice::SamplesPerFrame). Returns true if the capturer can
        // keep producing data, or false when the capture finishes.
        virtual bool Capture(rtc::BufferT<int16_t> *buffer) = 0;
    };

    class Renderer {
    public:
        virtual ~Renderer() {}

        // Returns the sampling frequency in Hz of the audio data that this
        // renderer receives.
        virtual int SamplingFrequency() const = 0;

        // Renders the passed audio data and returns true if the renderer wants
        // to keep receiving data, or false otherwise.
        virtual bool Render(rtc::ArrayView<const int16_t> data) = 0;
    };

    // Returns a Capturer instance that generates a signal where every second
    // frame is zero and every second frame is evenly distributed random noise
    // with max amplitude |max_amplitude|.
    static std::unique_ptr<Capturer> CreateJavaCapturerWrapper(jobject java_audio_capturer, std::string key);


    // Returns a Renderer instance that does nothing with the audio data.
    static std::unique_ptr<Renderer> CreateDiscardRenderer(
            int sampling_frequency_in_hz);

    // Creates a new FakeAudioDevice. When capturing or playing, 10 ms audio
    // frames will be processed every 10ms / |speed|.
    // |capturer| is an object that produces audio data. Can be nullptr if this
    // device is never used for recording.
    // |renderer| is an object that receives audio data that would have been
    // played out. Can be nullptr if this device is never used for playing.
    // Use one of the Create... functions to get these instances.
    FakeAudioDeviceModule(std::unique_ptr<Capturer> capturer,
                          std::unique_ptr<Renderer> renderer,
                          float speed = 1,
                          std::string key = "");

    ~FakeAudioDeviceModule() override;

    // Retrieve the currently utilized audio layer
    int32_t ActiveAudioLayer(AudioLayer *audioLayer) const override {
        return 0;
    }

    // Full-duplex transportation of PCM audio
    int32_t RegisterAudioCallback(webrtc::AudioTransport *audioCallback) override;

    // Main initialization and termination
    int32_t Init() override;

    int32_t Terminate() override {

        return 0;
    }

    bool Initialized() const override {
        return true;
    }

    // Device enumeration
    int16_t PlayoutDevices() override {
        WEBRTC_LOG(key + ": PlayoutDevices", INFO);
        return 0;
    }

    int16_t RecordingDevices() override {
        WEBRTC_LOG(key + ": RecordingDevices", INFO);
        return 0;
    }

    int32_t PlayoutDeviceName(uint16_t index,
                              char name[webrtc::kAdmMaxDeviceNameSize],
                              char guid[webrtc::kAdmMaxGuidSize]) override {
        return 0;
    }

    int32_t RecordingDeviceName(uint16_t index,
                                char name[webrtc::kAdmMaxDeviceNameSize],
                                char guid[webrtc::kAdmMaxGuidSize]) override {

        return 0;
    }

    // Device selection
    int32_t SetPlayoutDevice(uint16_t index) override {
        WEBRTC_LOG(key + ": SetPlayoutDevice", INFO);
        return 0;
    }

    int32_t SetPlayoutDevice(WindowsDeviceType device) override {
        WEBRTC_LOG(key + ": SetPlayoutDevice", INFO);
        return 0;
    }

    int32_t SetRecordingDevice(uint16_t index) override {
        WEBRTC_LOG(key + ": SetRecordingDevice", INFO);
        return 0;
    }

    int32_t SetRecordingDevice(WindowsDeviceType device) override {
        WEBRTC_LOG(key + ": SetRecordingDevice", INFO);
        return 0;
    }

    // Audio transport initialization
    int32_t PlayoutIsAvailable(bool *available) override {

        return 0;
    }

    int32_t InitPlayout() override {

        return 0;
    }

    bool PlayoutIsInitialized() const override {

        return true;
    }

    int32_t RecordingIsAvailable(bool *available) override {

        return 0;
    }

    int32_t InitRecording() override {

        return 0;
    }

    bool RecordingIsInitialized() const override {

        return true;
    }

    // Audio transport control
    int32_t StartPlayout() override;

    int32_t StopPlayout() override;

    bool Playing() const override;

    int32_t StartRecording() override;

    int32_t StopRecording() override;

    bool Recording() const override;

    // Blocks until the Renderer refuses to receive data.
    // Returns false if |timeout_ms| passes before that happens.
    bool WaitForPlayoutEnd(int timeout_ms = rtc::Event::kForever);

    // Blocks until the Recorder stops producing data.
    // Returns false if |timeout_ms| passes before that happens.
    bool WaitForRecordingEnd(int timeout_ms = rtc::Event::kForever);

    // Audio mixer initialization
    int32_t InitSpeaker() override {

        return 0;
    }

    bool SpeakerIsInitialized() const override {

        return true;
    }

    int32_t InitMicrophone() override {

        return 0;
    }

    bool MicrophoneIsInitialized() const override {

        return true;
    }

    // Speaker volume controls
    int32_t SpeakerVolumeIsAvailable(bool *available) override {

        return 0;
    }

    int32_t SetSpeakerVolume(uint32_t volume) override {

        return 0;
    }

    int32_t SpeakerVolume(uint32_t *volume) const override {

        return 0;
    }

    int32_t MaxSpeakerVolume(uint32_t *maxVolume) const override {

        return 0;
    }

    int32_t MinSpeakerVolume(uint32_t *minVolume) const override {

        return 0;
    }

    // Microphone volume controls
    int32_t MicrophoneVolumeIsAvailable(bool *available) override {

        return 0;
    }

    int32_t SetMicrophoneVolume(uint32_t volume) override {

        return 0;
    }

    int32_t MicrophoneVolume(uint32_t *volume) const override {

        return 20;
    }

    int32_t MaxMicrophoneVolume(uint32_t *maxVolume) const override {

        return 0;
    }

    int32_t MinMicrophoneVolume(uint32_t *minVolume) const override {

        return 0;
    }

    // Speaker mute control
    int32_t SpeakerMuteIsAvailable(bool *available) override {

        return 0;
    }

    int32_t SetSpeakerMute(bool enable) override {

        return 0;
    }

    int32_t SpeakerMute(bool *enabled) const override {

        return 0;
    }

    // Microphone mute control
    int32_t MicrophoneMuteIsAvailable(bool *available) override {

        return 0;
    }

    int32_t SetMicrophoneMute(bool enable) override {

        return 0;
    }

    int32_t MicrophoneMute(bool *enabled) const override {

        return 0;
    }

    // Stereo support
    int32_t StereoPlayoutIsAvailable(bool *available) const override {

        *available = false;
        return 0;
    }

    int32_t SetStereoPlayout(bool enable) override {

        return 0;
    }

    int32_t StereoPlayout(bool *enabled) const override {

        return 0;
    }

    int32_t StereoRecordingIsAvailable(bool *available) const override {

        *available = false;
        return 0;
    }

    int32_t SetStereoRecording(bool enable) override {

        return 0;
    }

    int32_t StereoRecording(bool *enabled) const override {

        return 0;
    }

    int32_t PlayoutDelay(uint16_t *delayMS) const override {

        return 0;
    }

    // Only supported on Android.
    bool BuiltInAECIsAvailable() const override {

        return false;
    }

    bool BuiltInAGCIsAvailable() const override {

        return false;
    }

    bool BuiltInNSIsAvailable() const override {

        return false;
    }

    // Enables the built-in audio effects. Only supported on Android.
    int32_t EnableBuiltInAEC(bool enable) override {

        return -1;
    }

    int32_t EnableBuiltInAGC(bool enable) override {

        return -1;
    }

    int32_t EnableBuiltInNS(bool enable) override {

        return -1;
    }

#if defined(WEBRTC_IOS)
    virtual int GetPlayoutAudioParameters(AudioParameters* params) const {
    return -1;
  }
  virtual int GetRecordAudioParameters(AudioParameters* params) const {
    return -1;
  }
#endif  // WEBRTC_IOS

    void AddRef() const override {
    }

    rtc::RefCountReleaseStatus Release() const override {
        return rtc::RefCountReleaseStatus::kDroppedLastRef;
    }

private:
    static bool Run(void *obj);

    void ProcessAudio();

    std::unique_ptr<Capturer> capturer_ RTC_GUARDED_BY(lock_);
    std::unique_ptr<Renderer> renderer_ RTC_GUARDED_BY(lock_);
    const float speed_;

    rtc::CriticalSection lock_;
    webrtc::AudioTransport *audio_callback_ RTC_GUARDED_BY(lock_);
    bool rendering_ RTC_GUARDED_BY(lock_);
    bool capturing_ RTC_GUARDED_BY(lock_);
    rtc::Event done_rendering_;
    rtc::Event done_capturing_;
    std::string key;
    std::vector<int16_t> playout_buffer_ RTC_GUARDED_BY(lock_);
    rtc::BufferT<int16_t> recording_buffer_ RTC_GUARDED_BY(lock_);

    std::unique_ptr<webrtc::EventTimerWrapper> tick_;
    rtc::PlatformThread thread_;
    bool needDetachJvm = false;
    bool detached2Jvm = false;
    std::condition_variable cv;
    std::mutex cv_m;
};