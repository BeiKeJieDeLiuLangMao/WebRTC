package bbm.webrtc.rtc4j.core.observer;

import bbm.webrtc.rtc4j.model.DataBuffer;

/**
 * @author bbm
 */
public interface DataChannelObserver {
    void onMessage(DataBuffer dataBuffer);
}
