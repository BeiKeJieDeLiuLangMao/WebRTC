//
// Created by 贝克街的流浪猫 on 2018-12-27.
//

#ifndef RTC_FFMPEG_H264_ENCODER_IMPL_H
#define RTC_FFMPEG_H264_ENCODER_IMPL_H


#include <common_video/h264/h264_bitstream_parser.h>
#include <fstream>
#include "h264.h"

extern "C" {
#include "libavutil/opt.h"
#include "libavcodec/avcodec.h"
#include "libavutil/channel_layout.h"
#include "libavutil/common.h"
#include "libavutil/imgutils.h"
#include "libavutil/mathematics.h"
#include "libavutil/samplefmt.h"
};

class FFmpegH264EncoderImpl : public H264Encoder {
public:
    typedef struct {
        AVCodec *codec = nullptr;        //指向编解码器实例
        AVFrame *frame = nullptr;        //保存解码之后/编码之前的像素数据
        AVCodecContext *context = nullptr;    //编解码器上下文，保存编解码器的一些参数设置
        AVPacket *pkt = nullptr;        //码流包结构，包含编码码流数据
    } CodecCtx;
public:
    FFmpegH264EncoderImpl(const cricket::VideoCodec &codec, bool hardware_accelerate, std::string key);

    ~FFmpegH264EncoderImpl() override;

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

private:
    bool OpenEncoder(CodecCtx *ctx, LayerConfig &io_param);

    void CloseEncoder(CodecCtx *ctx);

    void SetContext(CodecCtx *ctx, LayerConfig &io_param, bool init);

    void RtpFragmentize(webrtc::EncodedImage *encoded_image,
                        std::unique_ptr<uint8_t[]> *encoded_image_buffer,
                        const webrtc::VideoFrameBuffer &frame_buffer, AVPacket *packet,
                        webrtc::RTPFragmentationHeader *frag_header);

    void copyFrame(AVFrame *frame, const webrtc::I420BufferInterface *buffer);

    webrtc::FrameType ConvertToVideoFrameType(AVFrame *frame);

    webrtc::H264BitstreamParser h264_bitstream_parser_;

    // Reports statistics with histograms.
    void ReportInit();

    void ReportError();

    std::vector<CodecCtx *> encoders_;

    std::vector<LayerConfig> configurations_;
    std::vector<webrtc::EncodedImage> encoded_images_;
    std::vector<std::unique_ptr<uint8_t[]>> encoded_image_buffers_;

    webrtc::VideoCodec codec_;
    webrtc::H264PacketizationMode packetization_mode_;
    size_t max_payload_size_;
    int32_t number_of_cores_;
    webrtc::EncodedImageCallback *encoded_image_callback_;
    std::string key;
    bool has_reported_init_;
    bool hardware_accelerate;
    bool has_reported_error_;
};


#endif //RTC_FFMPEG_H264_ENCODER_IMPL_H
