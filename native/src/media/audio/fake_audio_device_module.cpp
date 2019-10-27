//
// Created by 贝克街的流浪猫 on 27/08/2018.
//
#include "fake_audio_device_module.h"
#include <algorithm>
#include <utility>
#include <thread>

#include "rtc_base/checks.h"
#include "rtc_base/random.h"
#include "common_audio/wav_file.h"
#include "system_wrappers/include/event_wrapper.h"
#include "jni/jni_utils.h"

constexpr int kFrameLengthMs = 10;
constexpr int kFramesPerSecond = 1000 / kFrameLengthMs;

class JavaAudioCapturerWrapper final : public FakeAudioDeviceModule::Capturer {
public:

    JavaAudioCapturerWrapper(jobject audio_capturer, std::string key)
            : java_audio_capturer(audio_capturer) {
        WEBRTC_LOG(key + ": Instance java audio capturer wrapper.", INFO);
        JNIEnv *env = ATTACH_CURRENT_THREAD_IF_NEEDED();
        audio_capture_class = env->GetObjectClass(java_audio_capturer);
        sampling_frequency_method = env->GetMethodID(audio_capture_class, "samplingFrequency", "()I");
        capture_method = env->GetMethodID(audio_capture_class, "capture", "(I)Ljava/nio/ByteBuffer;");
        WEBRTC_LOG(key + ": Instance java audio capturer wrapper end.", INFO);
    }

    ~JavaAudioCapturerWrapper() {
        JNIEnv *env = ATTACH_CURRENT_THREAD_IF_NEEDED();
        if (audio_capture_class != nullptr) {
            env->DeleteLocalRef(audio_capture_class);
            audio_capture_class = nullptr;
        }
        if (java_audio_capturer) {
            env->DeleteGlobalRef(java_audio_capturer);
            java_audio_capturer = nullptr;
        }
        if (audio_data_buffer) {
            env->DeleteGlobalRef(audio_data_buffer);
            audio_data_buffer = nullptr;
        }
    }

    bool isJavaWrapper() override {
        return true;
    }

    int SamplingFrequency() override {
        if (sampling_frequency_in_hz == 0) {
            JNIEnv *env = ATTACH_CURRENT_THREAD_IF_NEEDED();
            this->sampling_frequency_in_hz = env->CallIntMethod(java_audio_capturer, sampling_frequency_method);
        }
        return sampling_frequency_in_hz;
    }

    bool Capture(rtc::BufferT<int16_t> *buffer) override {
        buffer->SetData(
                FakeAudioDeviceModule::SamplesPerFrame(SamplingFrequency()),
                [&](rtc::ArrayView<int16_t> data) {
                    JNIEnv *env = ATTACH_CURRENT_THREAD_IF_NEEDED();
                    size_t feed_length = 0;
                    size_t need_length = data.size() * 2;
                    while(feed_length < need_length) {
                        if (audio_data_buffer == nullptr) {
                            jobject local_buffer = env->CallObjectMethod(java_audio_capturer, capture_method, need_length);
                            audio_data_buffer = env->NewGlobalRef(local_buffer);
                            env->DeleteLocalRef(local_buffer);
                            audio_data_address = env->GetDirectBufferAddress(audio_data_buffer);
                            audio_data_size = (size_t)env->GetDirectBufferCapacity(audio_data_buffer);
                            audio_buffer_index = 0;
                        }
                        size_t buffer_remain = audio_data_size - audio_buffer_index;
                        if (buffer_remain > (need_length - feed_length)) {
                            memcpy(&(data.data()[feed_length/2]), (char *)audio_data_address + audio_buffer_index, (need_length - feed_length));
                            audio_buffer_index += (need_length - feed_length);
                            feed_length += (need_length - feed_length);
                        } else {
                            memcpy(&(data.data()[feed_length/2]), (char *)audio_data_address + audio_buffer_index, buffer_remain);
                            audio_buffer_index += buffer_remain;
                            feed_length += buffer_remain;
                        }
                        if (audio_buffer_index == audio_data_size) {
                            env->DeleteGlobalRef(audio_data_buffer);
                            audio_data_buffer = nullptr;
                        }
                    }
                    return data.size();
                });
        return buffer->size() == buffer->capacity();
    }

private:
    jobject java_audio_capturer;
    jclass audio_capture_class;
    jmethodID sampling_frequency_method;
    jmethodID capture_method;
    int sampling_frequency_in_hz = 0;
    jobject audio_data_buffer = nullptr;
    void *audio_data_address;
    size_t audio_data_size;
    size_t audio_buffer_index = 0;
};

class DiscardRenderer final : public FakeAudioDeviceModule::Renderer {
public:
    explicit DiscardRenderer(int sampling_frequency_in_hz)
            : sampling_frequency_in_hz_(sampling_frequency_in_hz) {}

    int SamplingFrequency() const override {
        return sampling_frequency_in_hz_;
    }

    bool Render(rtc::ArrayView<const int16_t>) override {
        WEBRTC_LOG("Received a audio data", INFO);
        return true;
    }

private:
    int sampling_frequency_in_hz_;
};


size_t FakeAudioDeviceModule::SamplesPerFrame(int sampling_frequency_in_hz) {
    return rtc::CheckedDivExact(sampling_frequency_in_hz, kFramesPerSecond);
}

std::unique_ptr<FakeAudioDeviceModule::Capturer>
FakeAudioDeviceModule::CreateJavaCapturerWrapper(jobject java_audio_capturer, std::string key) {
    return std::unique_ptr<FakeAudioDeviceModule::Capturer>(new JavaAudioCapturerWrapper(java_audio_capturer, key));
}

std::unique_ptr<FakeAudioDeviceModule::Renderer>
FakeAudioDeviceModule::CreateDiscardRenderer(int sampling_frequency_in_hz) {
    return std::unique_ptr<FakeAudioDeviceModule::Renderer>(
            new DiscardRenderer(sampling_frequency_in_hz));
}


FakeAudioDeviceModule::FakeAudioDeviceModule(std::unique_ptr<Capturer> capturer,
                                             std::unique_ptr<Renderer> renderer,
                                             float speed,
                                             std::string key)
        : capturer_(std::move(capturer)),
          renderer_(std::move(renderer)),
          speed_(speed),
          audio_callback_(nullptr),
          rendering_(false),
          capturing_(false),
          done_rendering_(true, true),
          done_capturing_(true, true),
          tick_(webrtc::EventTimerWrapper::Create()),
          thread_(FakeAudioDeviceModule::Run, this, "FakeAudioDeviceModule") {
    this->key = std::move(key);
}

FakeAudioDeviceModule::~FakeAudioDeviceModule() {
    WEBRTC_LOG(key + ": In audio device module FakeAudioDeviceModule", INFO);
    StopPlayout();
    StopRecording();
    needDetachJvm = true;
    std::unique_lock<std::mutex> lk(cv_m);
    cv.wait(lk);
    WEBRTC_LOG(key + ": In audio device module after detached2Jvm", INFO);
    thread_.Stop();
    WEBRTC_LOG(key + ": In audio device module ~FakeAudioDeviceModule finished", INFO);
}

int32_t FakeAudioDeviceModule::StartPlayout() {
    rtc::CritScope cs(&lock_);
    RTC_CHECK(renderer_);
    rendering_ = true;
    done_rendering_.Reset();
    return 0;
}

int32_t FakeAudioDeviceModule::StopPlayout() {
    rtc::CritScope cs(&lock_);
    rendering_ = false;
    done_rendering_.Set();
    return 0;
}

int32_t FakeAudioDeviceModule::StartRecording() {
    rtc::CritScope cs(&lock_);
    WEBRTC_LOG(key + ": Start audio recording", INFO);
    RTC_CHECK(capturer_);
    capturing_ = true;
    done_capturing_.Reset();
    return 0;
}

int32_t FakeAudioDeviceModule::StopRecording() {
    rtc::CritScope cs(&lock_);
    WEBRTC_LOG(key + ": Stop audio recording", INFO);
    capturing_ = false;
    done_capturing_.Set();
    return 0;
}

int32_t FakeAudioDeviceModule::Init() {
    RTC_CHECK(tick_->StartTimer(true, kFrameLengthMs / speed_));
    thread_.Start();
    thread_.SetPriority(rtc::kHighPriority);
    return 0;
}

int32_t FakeAudioDeviceModule::RegisterAudioCallback(webrtc::AudioTransport *callback) {
    rtc::CritScope cs(&lock_);
    RTC_DCHECK(callback || audio_callback_);
    audio_callback_ = callback;
    return 0;
}

bool FakeAudioDeviceModule::Playing() const {
    rtc::CritScope cs(&lock_);
    return rendering_;
}

bool FakeAudioDeviceModule::Recording() const {
    rtc::CritScope cs(&lock_);
    return capturing_;
}

bool FakeAudioDeviceModule::WaitForPlayoutEnd(int timeout_ms) {
    return done_rendering_.Wait(timeout_ms);
}

bool FakeAudioDeviceModule::WaitForRecordingEnd(int timeout_ms) {
    return done_capturing_.Wait(timeout_ms);
}

bool FakeAudioDeviceModule::Run(void *obj) {
    static_cast<FakeAudioDeviceModule *>(obj)->ProcessAudio();
    return true;
}

void FakeAudioDeviceModule::ProcessAudio() {
    {
        rtc::CritScope cs(&lock_);
        if (needDetachJvm) {
            WEBRTC_LOG(key + ": In audio device module process audio", INFO);
        }
        auto start = std::chrono::steady_clock::now();
        if (capturing_) {
            // Capture 10ms of audio. 2 bytes per sample.
            const bool keep_capturing = capturer_->Capture(&recording_buffer_);
            uint32_t new_mic_level;
            if (keep_capturing) {
                audio_callback_->RecordedDataIsAvailable(
                        recording_buffer_.data(), recording_buffer_.size(), 2, 1,
                        static_cast<const uint32_t>(capturer_->SamplingFrequency()), 0, 0, 0, false, new_mic_level);
            }
            if (!keep_capturing) {
                capturing_ = false;
                done_capturing_.Set();
            }
        }
        if (rendering_) {
            size_t samples_out;
            int64_t elapsed_time_ms;
            int64_t ntp_time_ms;
            const int sampling_frequency = renderer_->SamplingFrequency();
            audio_callback_->NeedMorePlayData(
                    SamplesPerFrame(sampling_frequency), 2, 1, static_cast<const uint32_t>(sampling_frequency),
                    playout_buffer_.data(), samples_out, &elapsed_time_ms, &ntp_time_ms);
            const bool keep_rendering = renderer_->Render(
                    rtc::ArrayView<const int16_t>(playout_buffer_.data(), samples_out));
            if (!keep_rendering) {
                rendering_ = false;
                done_rendering_.Set();
            }
        }
        auto end = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration<double, std::milli>(end - start).count();
        if (diff > kFrameLengthMs) {
            WEBRTC_LOG(key + ": JNI capture audio data timeout, real capture time is " + std::to_string(diff) + " ms", DEBUG);
        }
        if (capturer_->isJavaWrapper() && needDetachJvm && !detached2Jvm) {
            DETACH_CURRENT_THREAD_IF_NEEDED();
            detached2Jvm = true;
            cv.notify_all();
        } else if (needDetachJvm) {
            detached2Jvm = true;
            cv.notify_all();
        }
    }
    tick_->Wait(WEBRTC_EVENT_INFINITE);
}
