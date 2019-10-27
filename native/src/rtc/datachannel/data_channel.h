//
// Created by 贝克街的流浪猫 on 18/07/2018.
//

#ifndef RTC_DATACHANNEL_H
#define RTC_DATACHANNEL_H


#include "rtc/rtc_observer.h"

class DataChannel {

public:
    DataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel,
                DataChannelObserver *data_channel_observer);

    explicit DataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel);

    void Send(webrtc::DataBuffer &data_buffer);

    bool isOpen();

    std::string label();

    void RegisterObserver(DataChannelObserver *observer);

    ~DataChannel();

private:
    rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel;
    DataChannelObserver *data_channel_observer;
};


#endif //RTC_DATACHANNEL_H
