package bbm.webrtc.rtc4j.core.observer;

import bbm.webrtc.rtc4j.model.SessionDescription;

/**
 * @author bbm
 */
public interface SessionDescriptionObserver {
    void OnSuccess(SessionDescription sdp);
}
