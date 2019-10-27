//
// Created by 贝克街的流浪猫 on 21/11/2018.
//

#include <rtc_base/arraysize.h>
#include <iostream>
#include <rtc_base/thread.h>
#include "api/video/i420_buffer.h"
#include "fake_video_capturer.h"
#include "jni/jni_utils.h"

FakeVideoCapturer::FakeVideoCapturer(jobject video_capturer, std::string key)
        : running_(false),
          video_capturer(video_capturer),
          is_screen_cast(false),
          ticker(webrtc::EventTimerWrapper::Create()),
          thread(FakeVideoCapturer::Run, this, "FakeVideoCapturer") {
    this->key = std::move(key);
    JNIEnv *env = ATTACH_CURRENT_THREAD_IF_NEEDED();
    video_capture_class = env->GetObjectClass(video_capturer);
    get_width_method = env->GetMethodID(video_capture_class, "getWidth", "()I");
    get_height_method = env->GetMethodID(video_capture_class, "getHeight", "()I");
    get_fps_method = env->GetMethodID(video_capture_class, "getFps", "()I");
    capture_method = env->GetMethodID(video_capture_class, "capture", "()Lbbm/webrtc/rtc4j/model/VideoFrame;");
    width = env->CallIntMethod(video_capturer, get_width_method);
    previous_width = width;
    height = env->CallIntMethod(video_capturer, get_height_method);
    previous_height = height;
    fps = env->CallIntMethod(video_capturer, get_fps_method);
    static const cricket::VideoFormat formats[] = {
            {width, height, cricket::VideoFormat::FpsToInterval(fps), cricket::FOURCC_I420}
    };
    SetSupportedFormats({&formats[0], &formats[arraysize(formats)]});
    RTC_CHECK(ticker->StartTimer(true, rtc::kNumMillisecsPerSec / fps));
    thread.Start();
    thread.SetPriority(rtc::kHighPriority);
    decompress_handle = tjInitDecompress();
    WEBRTC_LOG(key + ": Create fake video capturer, " + std::to_string(width) + ", " + std::to_string(height), INFO);
}

FakeVideoCapturer::~FakeVideoCapturer() {
    needDetachJvm = true;
    std::unique_lock<std::mutex> lk(cv_m);
    cv.wait(lk);
    thread.Stop();
    SignalDestroyed(this);
    JNIEnv *env = ATTACH_CURRENT_THREAD_IF_NEEDED();
    if (video_capture_class != nullptr) {
        env->DeleteLocalRef(video_capture_class);
        video_capture_class = nullptr;
    }
    if (decompress_handle) {
        if (tjDestroy(decompress_handle) != 0) {
            WEBRTC_LOG(key + ": Release decompress handle failed, reason is: " + std::string(tjGetErrorStr2(decompress_handle)),
                       ERROR);
        }
    }
    WEBRTC_LOG(key + ": Free fake video capturer", INFO);
}

bool FakeVideoCapturer::Run(void *obj) {
    static_cast<FakeVideoCapturer *>(obj)->CaptureFrame();
    return true;
}

void FakeVideoCapturer::CaptureFrame() {
    {
        rtc::CritScope cs(&lock_);
        if (running_) {
            int64_t t0 = rtc::TimeMicros();
            JNIEnv *env = ATTACH_CURRENT_THREAD_IF_NEEDED();
            jobject java_video_frame = env->CallObjectMethod(video_capturer, capture_method);
            if (java_video_frame == nullptr) {
                rtc::scoped_refptr<webrtc::I420Buffer> buffer = webrtc::I420Buffer::Create(previous_width,
                                                                                           previous_height);
                webrtc::I420Buffer::SetBlack(buffer);
                OnFrame(webrtc::VideoFrame(buffer, (webrtc::VideoRotation) previous_rotation, t0), previous_width,
                        previous_height);
                return;
            }
            jobject java_data_buffer = env->CallObjectMethod(java_video_frame, GET_VIDEO_FRAME_BUFFER_GETTER_METHOD());
            auto data_buffer = (unsigned char *) env->GetDirectBufferAddress(java_data_buffer);
            auto length = (unsigned long) env->CallIntMethod(java_video_frame, GET_VIDEO_FRAME_LENGTH_GETTER_METHOD());
            int rotation = 0;
            int width;
            int height;
            tjDecompressHeader(decompress_handle, data_buffer, length, &width, &height);
            previous_width = width;
            previous_height = height;
            previous_rotation = rotation;
            rtc::scoped_refptr<webrtc::I420Buffer> buffer =
                    webrtc::I420Buffer::Create(width, height,
                                               width % 32 == 0 ? width : width / 32 * 32 + 32,
                                               (width / 2) % 32 == 0 ? (width / 2) : (width / 2) / 32 * 32 + 32,
                                               (width / 2) % 32 == 0 ? (width / 2) : (width / 2) / 32 * 32 + 32);
            uint8_t *planes[] = {buffer->MutableDataY(), buffer->MutableDataU(), buffer->MutableDataV()};
            int strides[] = {buffer->StrideY(), buffer->StrideU(), buffer->StrideV()};
            tjDecompressToYUVPlanes(decompress_handle, data_buffer, length, planes, width, strides, height,
                                    TJFLAG_FASTDCT | TJFLAG_NOREALLOC);
            env->DeleteLocalRef(java_data_buffer);
            env->DeleteLocalRef(java_video_frame);
            OnFrame(webrtc::VideoFrame(buffer, (webrtc::VideoRotation) rotation, t0), width, height);
        }
        if (needDetachJvm && !detached2Jvm) {
            DETACH_CURRENT_THREAD_IF_NEEDED();
            detached2Jvm = true;
            cv.notify_all();
        } else if (needDetachJvm) {
            detached2Jvm = true;
            cv.notify_all();
        }
    }
    ticker->Wait(WEBRTC_EVENT_INFINITE);
}

cricket::CaptureState FakeVideoCapturer::Start(
        const cricket::VideoFormat &format) {
    //SetCaptureFormat(&format); This will cause crash in CentOS
    running_ = true;
    SetCaptureState(cricket::CS_RUNNING);
    WEBRTC_LOG(key + ": Start fake video capturing", INFO);
    return cricket::CS_RUNNING;
}

void FakeVideoCapturer::Stop() {
    running_ = false;
    //SetCaptureFormat(nullptr); This will cause crash in CentOS
    SetCaptureState(cricket::CS_STOPPED);
    WEBRTC_LOG(key + ":Stop fake video capturing", INFO);
}

bool FakeVideoCapturer::IsRunning() {
    return running_;
}

bool FakeVideoCapturer::IsScreencast() const {
    return is_screen_cast;
}

bool FakeVideoCapturer::GetPreferredFourccs(std::vector<uint32_t> *fourccs) {
    fourccs->push_back(cricket::FOURCC_I420);
    return true;
}

void FakeVideoCapturer::AddOrUpdateSink(rtc::VideoSinkInterface<webrtc::VideoFrame> *sink,
                                        const rtc::VideoSinkWants &wants) {
    cricket::VideoCapturer::AddOrUpdateSink(sink, wants);
}

void FakeVideoCapturer::RemoveSink(rtc::VideoSinkInterface<webrtc::VideoFrame> *sink) {
    cricket::VideoCapturer::RemoveSink(sink);
}


