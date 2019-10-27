//
// Created by 贝克街的流浪猫 on 27/11/2018.
//

#include <absl/memory/memory.h>
#include <media/base/mediaconstants.h>
#include <media/base/codec.h>
#include "media/base/vp9_profile.h"
#include "media/base/h264_profile_level_id.h"
#include "video_encoder_decoder_factory.h"
#include "api/video_codecs/sdp_video_format.h"
#include "modules/video_coding/codecs/vp8/include/vp8.h"
#include "modules/video_coding/codecs/vp9/include/vp9.h"
#include "jni/jni_utils.h"
#include "media/codec/h264/h264.h"

webrtc::SdpVideoFormat CreateH264Format(webrtc::H264::Profile profile,
                                        webrtc::H264::Level level,
                                        const std::string &packetization_mode) {
    const absl::optional<std::string> profile_string =
            webrtc::H264::ProfileLevelIdToString(webrtc::H264::ProfileLevelId(profile, level));
    RTC_CHECK(profile_string);
    return webrtc::SdpVideoFormat(cricket::kH264CodecName,
                                  {{cricket::kH264FmtpProfileLevelId,        *profile_string},
                                   {cricket::kH264FmtpLevelAsymmetryAllowed, "1"},
                                   {cricket::kH264FmtpPacketizationMode,     packetization_mode}});
}

std::vector<webrtc::SdpVideoFormat> GetAllSupportedFormats() {
    std::vector<webrtc::SdpVideoFormat> supported_codecs;

    supported_codecs.emplace_back(CreateH264Format(webrtc::H264::kProfileBaseline, webrtc::H264::kLevel3_1, "1"));
    // Need a constrained baseline h264 support, because safari only support this type of profile.
    supported_codecs.emplace_back(CreateH264Format(webrtc::H264::kProfileConstrainedBaseline, webrtc::H264::kLevel3_1, "1"));
    supported_codecs.emplace_back(webrtc::SdpVideoFormat(cricket::kVp9CodecName,
                                                         {{webrtc::kVP9FmtpProfileId, VP9ProfileToString(
                                                                 webrtc::VP9Profile::kProfile0)}}));
    supported_codecs.emplace_back(cricket::kVp8CodecName);
    return supported_codecs;
}

class VideoEncoderFactory : public webrtc::VideoEncoderFactory {

public:
    VideoEncoderFactory(bool hardware, std::string key) : hardware_accelerate(hardware) {
        this->key = std::move(key);
    }

    // Returns a list of supported video formats in order of preference, to use
    // for signaling etc.
    std::vector<webrtc::SdpVideoFormat> GetSupportedFormats() const override {
        return GetAllSupportedFormats();
    }

    // Returns information about how this format will be encoded. The specified
    // format must be one of the supported formats by this factory.
    CodecInfo QueryVideoEncoder(const webrtc::SdpVideoFormat &format) const override {
        CodecInfo info{};
        info.is_hardware_accelerated = false;
        info.has_internal_source = false;
        return info;
    }

    // Creates a VideoEncoder for the specified format.
    std::unique_ptr<webrtc::VideoEncoder> CreateVideoEncoder(
            const webrtc::SdpVideoFormat &format) override {
        WEBRTC_LOG(key + ": Trying to created encoder of format " + format.name, INFO);
        if (cricket::CodecNamesEq(format.name, cricket::kVp8CodecName))
            return webrtc::VP8Encoder::Create();
        if (cricket::CodecNamesEq(format.name, cricket::kVp9CodecName))
            return webrtc::VP9Encoder::Create(cricket::VideoCodec(format));
        if (cricket::CodecNamesEq(format.name, cricket::kH264CodecName))
            return H264Encoder::Create(cricket::VideoCodec(format), hardware_accelerate, key);
        return nullptr;
    }

    ~VideoEncoderFactory() override = default;

private:
    bool hardware_accelerate;
    std::string key;
};

class VideoDecoderFactory : public webrtc::VideoDecoderFactory {

public:
    VideoDecoderFactory(std::string key) {
        this->key = std::move(key);
    }

    // Returns a list of supported video formats in order of preference, to use
    // for signaling etc.
    std::vector<webrtc::SdpVideoFormat> GetSupportedFormats() const override {
        return GetAllSupportedFormats();
    }

    // Creates a VideoDecoder for the specified format.
    std::unique_ptr<webrtc::VideoDecoder> CreateVideoDecoder(
            const webrtc::SdpVideoFormat &format) override {
        if (cricket::CodecNamesEq(format.name, cricket::kVp8CodecName))
            return webrtc::VP8Decoder::Create();
        if (cricket::CodecNamesEq(format.name, cricket::kVp9CodecName))
            return webrtc::VP9Decoder::Create();
        if (cricket::CodecNamesEq(format.name, cricket::kH264CodecName))
            return H264Decoder::Create();
        WEBRTC_LOG(key + ": Trying to created encoder of unsupported format " + format.name, WARNING);
        return nullptr;
    }

    ~VideoDecoderFactory() override = default;

private:
    std::string key;
};


std::unique_ptr<webrtc::VideoEncoderFactory> CreateVideoEncoderFactory(bool hardware_accelerate, std::string key) {
    return absl::make_unique<VideoEncoderFactory>(hardware_accelerate, key);
}

std::unique_ptr<webrtc::VideoDecoderFactory> CreateVideoDecoderFactory(std::string key) {
    return absl::make_unique<VideoDecoderFactory>(key);
}