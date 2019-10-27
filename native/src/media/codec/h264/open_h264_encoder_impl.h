//
// Created by 贝克街的流浪猫 on 2018-12-27.
//

#ifndef RTC_H264_ENCODER_IMPL_H
#define RTC_H264_ENCODER_IMPL_H


#include <common_video/h264/h264_bitstream_parser.h>
#include "h264.h"
#include <memory>
#include <vector>

#include "api/video/i420_buffer.h"
#include "common_video/h264/h264_bitstream_parser.h"
#include "modules/video_coding/codecs/h264/include/h264.h"
#include "modules/video_coding/utility/quality_scaler.h"

#include "third_party/openh264/src/codec/api/svc/codec_app_def.h"

class ISVCEncoder;

class OpenH264EncoderImpl : public H264Encoder {

public:
    explicit OpenH264EncoderImpl(const cricket::VideoCodec &codec);

    ~OpenH264EncoderImpl() override;

    // |max_payload_size| is ignored.
    // The following members of |codec_settings| are used. The rest are ignored.
    // - codecType (must be kVideoCodecH264)
    // - targetBitrate
    // - maxFramerate
    // - width
    // - height
    int32_t InitEncode(const webrtc::VideoCodec *codec_settings,
                       int32_t number_of_cores,
                       size_t max_payload_size) override;

    int32_t Release() override;

    int32_t RegisterEncodeCompleteCallback(
            webrtc::EncodedImageCallback *callback) override;

    int32_t SetRateAllocation(const webrtc::VideoBitrateAllocation &bitrate_allocation,
                              uint32_t framerate) override;

    // The result of encoding - an EncodedImage and RTPFragmentationHeader - are
    // passed to the encode complete callback.
    int32_t Encode(const webrtc::VideoFrame &frame,
                   const webrtc::CodecSpecificInfo *codec_specific_info,
                   const std::vector<webrtc::FrameType> *frame_types) override;

    const char *ImplementationName() const override;

    VideoEncoder::ScalingSettings GetScalingSettings() const override;

    // Unsupported / Do nothing.
    int32_t SetChannelParameters(uint32_t packet_loss, int64_t rtt) override;

    // Exposed for testing.
    webrtc::H264PacketizationMode PacketizationModeForTesting() const {
        return packetization_mode_;
    }

private:
    SEncParamExt CreateEncoderParams(size_t i);

    webrtc::H264BitstreamParser h264_bitstream_parser_;

    // Reports statistics with histograms.
    void ReportInit();

    void ReportError();

    void RtpFragmentize(webrtc::EncodedImage *encoded_image,
                        std::unique_ptr<uint8_t[]> *encoded_image_buffer,
                        const webrtc::VideoFrameBuffer &frame_buffer,
                        SFrameBSInfo *info,
                        webrtc::RTPFragmentationHeader *frag_header);

    webrtc::FrameType ConvertToVideoFrameType(EVideoFrameType type);

    unsigned short NumberOfThreads(int width, int height, int number_of_cores);

    std::vector<ISVCEncoder *> encoders_;
    std::vector<SSourcePicture> pictures_;
    std::vector<LayerConfig> configurations_;
    std::vector<webrtc::EncodedImage> encoded_images_;
    std::vector<std::unique_ptr<uint8_t[]>> encoded_image_buffers_;

    webrtc::VideoCodec codec_;
    webrtc::H264PacketizationMode packetization_mode_;
    size_t max_payload_size_;
    int32_t number_of_cores_;
    webrtc::EncodedImageCallback *encoded_image_callback_;

    bool has_reported_init_;
    bool has_reported_error_;
};


#endif //RTC_H264_ENCODER_IMPL_H
