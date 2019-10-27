package bbm.webrtc.rtc4j.core.observer;

import bbm.webrtc.rtc4j.model.IceCandidate;
import bbm.webrtc.rtc4j.core.DataChannel;

/**
 * @author bbm
 */
public interface PeerConnectionObserver {

    void onIceCandidate(IceCandidate iceCandidate);

    void onSignalingChange(int state);

    void onDataChannel(DataChannel dataChannel);

    void onRenegotiationNeeded();

}
