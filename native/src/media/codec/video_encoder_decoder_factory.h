//
// Created by 贝克街的流浪猫 on 27/11/2018.
//

#ifndef RTC_VIDEOENCODERDECODERFACTORY_H
#define RTC_VIDEOENCODERDECODERFACTORY_H


#include <api/video_codecs/video_encoder_factory.h>
#include <api/video_codecs/video_decoder_factory.h>

std::unique_ptr<webrtc::VideoEncoderFactory> CreateVideoEncoderFactory(bool hardware_accelerate, std::string key);

std::unique_ptr<webrtc::VideoDecoderFactory> CreateVideoDecoderFactory(std::string key);


#endif //RTC_VIDEOENCODERDECODERFACTORY_H
