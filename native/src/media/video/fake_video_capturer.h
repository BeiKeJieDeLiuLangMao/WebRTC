//
// Created by 贝克街的流浪猫 on 21/11/2018.
//

#ifndef RTC_FAKEVIDEOCAPTURER_H
#define RTC_FAKEVIDEOCAPTURER_H

#include <media/base/videocapturer.h>
#include <system_wrappers/include/event_wrapper.h>
#include <rtc_base/platform_thread.h>
#include <jni.h>
extern "C" {
#include "turbojpeg.h"
}
#include <mutex>
#include <condition_variable>

namespace cricket {

    struct VideoFormat;

}  // namespace cricket

namespace webrtc {

    class VideoFrame;

}  // namespace webrtc


class FakeVideoCapturer : public cricket::VideoCapturer {
public:
    FakeVideoCapturer(jobject video_capturer, std::string key);

    ~FakeVideoCapturer() override;

    sigslot::signal1<FakeVideoCapturer *> SignalDestroyed;

    cricket::CaptureState Start(const cricket::VideoFormat &format) override;

    void Stop() override;

    bool IsRunning() override;

    bool IsScreencast() const override;

    bool GetPreferredFourccs(std::vector<uint32_t> *fourccs) override;

    void AddOrUpdateSink(rtc::VideoSinkInterface<webrtc::VideoFrame> *sink,
                         const rtc::VideoSinkWants &wants) override;

    void RemoveSink(rtc::VideoSinkInterface<webrtc::VideoFrame> *sink) override;

private:
    static bool Run(void *obj);

    void CaptureFrame();

    bool running_;
    rtc::CriticalSection lock_;
    rtc::PlatformThread thread;
    std::unique_ptr<webrtc::EventTimerWrapper> ticker;
    const bool is_screen_cast;
    int width;
    int height;
    int fps;
    std::string key;
    jobject video_capturer;
    jclass video_capture_class;
    jmethodID get_width_method;
    jmethodID get_height_method;
    jmethodID get_fps_method;
    jmethodID capture_method;
    int previous_width;
    int previous_height;
    int previous_rotation = 0;
    bool needDetachJvm = false;
    bool detached2Jvm = false;
    std::condition_variable cv;
    std::mutex cv_m;
    tjhandle decompress_handle;
};


#endif //RTC_FAKEVIDEOCAPTURER_H
