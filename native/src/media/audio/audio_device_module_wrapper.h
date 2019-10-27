//
// Created by 贝克街的流浪猫 on 07/08/2018.
//

#ifndef RTC_AUDIODEVICEMODULEWRAPPER_H
#define RTC_AUDIODEVICEMODULEWRAPPER_H

#include <modules/audio_device/audio_device_impl.h>
#include <modules/audio_device/include/audio_device.h>

class audio_device_module_wrapper : public webrtc::AudioDeviceModule {
public:
    audio_device_module_wrapper(const AudioLayer audioLayer) {
        this->inter = webrtc::AudioDeviceModule::Create((1 << 16) + 99, audioLayer);
    }

    void SetDeviceId(uint16_t deviceId) {
        this->deviceId = deviceId;
    }

    // Retrieve the currently utilized audio layer
    int32_t ActiveAudioLayer(AudioLayer *audioLayer) const override {
        return this->inter->ActiveAudioLayer(audioLayer);
    }

    // Full-duplex transportation of PCM audio
    int32_t RegisterAudioCallback(webrtc::AudioTransport *audioCallback) override {
        return this->inter->RegisterAudioCallback(audioCallback);
    }

    // Main initialization and termination
    int32_t Init() override {
        return this->inter->Init();
    }

    int32_t Terminate() override {
        return this->inter->Terminate();
    }

    bool Initialized() const override {
        return this->inter->Initialized();
    }

    // Device enumeration
    int16_t PlayoutDevices() override {
        return this->inter->PlayoutDevices();
    }

    int16_t RecordingDevices() override {
        return this->inter->RecordingDevices();
    }

    int32_t PlayoutDeviceName(uint16_t index,
                              char name[webrtc::kAdmMaxDeviceNameSize],
                              char guid[webrtc::kAdmMaxGuidSize]) override {
        return this->inter->PlayoutDeviceName(index, name, guid);
    }

    int32_t RecordingDeviceName(uint16_t index,
                                char name[webrtc::kAdmMaxDeviceNameSize],
                                char guid[webrtc::kAdmMaxGuidSize]) override {
        return this->inter->RecordingDeviceName(index, name, guid);
    }

    // Device selection
    int32_t SetPlayoutDevice(uint16_t index) override {
        return this->inter->SetPlayoutDevice(index);
    }

    int32_t SetPlayoutDevice(WindowsDeviceType device) override {
        return this->inter->SetPlayoutDevice(device);
    }

    int32_t SetRecordingDevice(uint16_t index) override {
        return this->inter->SetRecordingDevice(static_cast<uint16_t>(deviceId));
    }

    int32_t SetRecordingDevice(WindowsDeviceType device) override {
        return this->inter->SetRecordingDevice(device);
    }

    // Audio transport initialization
    int32_t PlayoutIsAvailable(bool *available) override {
        return this->inter->PlayoutIsAvailable(available);
    }

    int32_t InitPlayout() override {
        return this->inter->InitPlayout();
    }

    bool PlayoutIsInitialized() const override {
        return this->inter->PlayoutIsInitialized();
    }

    int32_t RecordingIsAvailable(bool *available) override {
        return this->inter->RecordingIsAvailable(available);
    }

    int32_t InitRecording() override {
        return this->inter->InitRecording();
    }

    bool RecordingIsInitialized() const override {
        return this->inter->RecordingIsInitialized();
    }

    // Audio transport control
    int32_t StartPlayout() override {
        return this->inter->StartPlayout();
    }

    int32_t StopPlayout() override {
        return this->inter->StopPlayout();
    }

    bool Playing() const override {
        return this->inter->Playing();
    }

    int32_t StartRecording() override {
        return this->inter->StartRecording();
    }

    int32_t StopRecording() override {
        return this->inter->StopRecording();
    }

    bool Recording() const override {
        return this->inter->Recording();
    }

    // Audio mixer initialization
    int32_t InitSpeaker() override {
        return this->inter->InitSpeaker();
    }

    bool SpeakerIsInitialized() const override {
        return this->inter->SpeakerIsInitialized();
    }

    int32_t InitMicrophone() override {
        return this->inter->InitMicrophone();
    }

    bool MicrophoneIsInitialized() const override {
        return this->inter->MicrophoneIsInitialized();
    }

    // Speaker volume controls
    int32_t SpeakerVolumeIsAvailable(bool *available) override {
        return this->inter->SpeakerVolumeIsAvailable(available);
    }

    int32_t SetSpeakerVolume(uint32_t volume) override {
        return this->inter->SetSpeakerVolume(volume);
    }

    int32_t SpeakerVolume(uint32_t *volume) const override {
        return this->inter->SpeakerVolume(volume);
    }

    int32_t MaxSpeakerVolume(uint32_t *maxVolume) const override {
        return this->inter->MaxSpeakerVolume(maxVolume);
    }

    int32_t MinSpeakerVolume(uint32_t *minVolume) const override {
        return this->inter->MinSpeakerVolume(minVolume);
    }


    // Microphone volume controls
    int32_t MicrophoneVolumeIsAvailable(bool *available) override {
        return this->inter->MicrophoneVolumeIsAvailable(available);
    }

    int32_t SetMicrophoneVolume(uint32_t volume) override {
        return this->inter->SetMicrophoneVolume(volume);
    }

    int32_t MicrophoneVolume(uint32_t *volume) const override {
        return this->inter->MicrophoneVolume(volume);
    }

    int32_t MaxMicrophoneVolume(uint32_t *maxVolume) const override {
        return this->inter->MaxMicrophoneVolume(maxVolume);
    }

    int32_t MinMicrophoneVolume(uint32_t *minVolume) const override {
        return this->inter->MinMicrophoneVolume(minVolume);
    }

    // Speaker mute control
    int32_t SpeakerMuteIsAvailable(bool *available) override {
        return this->inter->SpeakerMuteIsAvailable(available);
    }

    int32_t SetSpeakerMute(bool enable) override {
        return this->inter->SetSpeakerMute(enable);
    }

    int32_t SpeakerMute(bool *enabled) const override {
        return this->inter->SpeakerMute(enabled);
    }

    // Microphone mute control
    int32_t MicrophoneMuteIsAvailable(bool *available) override {
        return this->inter->MicrophoneMuteIsAvailable(available);
    }

    int32_t SetMicrophoneMute(bool enable) override {
        return this->inter->SetMicrophoneMute(enable);
    }

    int32_t MicrophoneMute(bool *enabled) const override {
        return this->inter->MicrophoneMute(enabled);
    }

    // Stereo support
    int32_t StereoPlayoutIsAvailable(bool *available) const override {
        return this->inter->StereoPlayoutIsAvailable(available);
    }

    int32_t SetStereoPlayout(bool enable) override {
        return this->inter->SetStereoPlayout(enable);
    }

    int32_t StereoPlayout(bool *enabled) const override {
        return this->inter->StereoPlayout(enabled);
    }

    int32_t StereoRecordingIsAvailable(bool *available) const override {
        return this->inter->StereoRecordingIsAvailable(available);
    }

    int32_t SetStereoRecording(bool enable) override {
        return this->inter->SetStereoRecording(enable);
    }

    int32_t StereoRecording(bool *enabled) const override {
        return this->inter->StereoRecording(enabled);
    }


    int32_t PlayoutDelay(uint16_t *delayMS) const override {
        return this->inter->PlayoutDelay(delayMS);
    }

    // Only supported on Android.
    bool BuiltInAECIsAvailable() const override {
        return this->inter->BuiltInAECIsAvailable();
    }

    bool BuiltInAGCIsAvailable() const override {
        return this->inter->BuiltInAGCIsAvailable();
    }

    bool BuiltInNSIsAvailable() const override {
        return this->inter->BuiltInNSIsAvailable();
    }

    // Enables the built-in audio effects. Only supported on Android.
    int32_t EnableBuiltInAEC(bool enable) override {
        return this->inter->EnableBuiltInAEC(enable);
    }

    int32_t EnableBuiltInAGC(bool enable) override {
        return this->inter->EnableBuiltInAGC(enable);
    }

    int32_t EnableBuiltInNS(bool enable) override {
        return this->inter->EnableBuiltInNS(enable);
    }

    void AddRef() const override {
        return this->inter->AddRef();
    }

    rtc::RefCountReleaseStatus Release() const override {
        return this->inter->Release();
    }

private:
    int16_t deviceId;
    rtc::scoped_refptr<AudioDeviceModule> inter;
};

#endif //RTC_AUDIODEVICEMODULEWRAPPER_H