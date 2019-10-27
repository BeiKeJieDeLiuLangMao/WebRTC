//
// Created by 贝克街的流浪猫 on 2018-12-27.
//

#ifndef RTC_H264_H
#define RTC_H264_H

#include <modules/video_coding/codecs/h264/include/h264.h>
#include "media/base/codec.h"
#include "modules/video_coding/include/video_codec_interface.h"

class H264Encoder : public webrtc::VideoEncoder {

public:
    struct LayerConfig {
        int simulcast_idx = 0;
        int width = -1;
        int height = -1;
        bool sending = true;
        bool key_frame_request = false;
        float max_frame_rate = 0;
        uint32_t target_bps = 0;
        uint32_t max_bps = 0;
        bool frame_dropping_on = false;
        int key_frame_interval = 0;

        void SetStreamState(bool send_stream);
    };

public:
    static std::unique_ptr<H264Encoder> Create(const cricket::VideoCodec &codec, bool hardware_accelerate, std::string key);

    ~H264Encoder() override {}
};

class H264Decoder : public webrtc::VideoDecoder {
public:
    static std::unique_ptr<webrtc::H264Decoder> Create();

    ~H264Decoder() override {}
};


#endif //RTC_H264_H
