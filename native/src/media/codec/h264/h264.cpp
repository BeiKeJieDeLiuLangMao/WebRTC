//
// Created by 贝克街的流浪猫 on 2018-12-27.
//

#include "h264.h"
#include "modules/video_coding/codecs/h264/h264_decoder_impl.h"
#include "ffmpeg_h264_encoder_impl.h"

#include "absl/memory/memory.h"
#include "open_h264_encoder_impl.h"

std::unique_ptr<H264Encoder> H264Encoder::Create(
        const cricket::VideoCodec &codec, bool hardware_accelerate, std::string key) {
    return absl::make_unique<FFmpegH264EncoderImpl>(codec, hardware_accelerate, key);
}

std::unique_ptr<webrtc::H264Decoder> H264Decoder::Create() {
    return absl::make_unique<webrtc::H264DecoderImpl>();
}

void H264Encoder::LayerConfig::SetStreamState(bool send_stream) {
    if (send_stream && !sending) {
        // Need a key frame if we have not sent this stream before.
        key_frame_request = true;
    }
    sending = send_stream;
}