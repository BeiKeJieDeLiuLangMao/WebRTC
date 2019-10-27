package bbm.webrtc.rtc4j.model;

import lombok.Getter;

/**
 * @author bbm
 */

public enum SignalingState {

    /**
     * Peer Connection Signaling State
     */
    Stable(0),
    HaveLocalOffer(1),
    HaveLocalPrAnswer(2),
    HaveRemoteOffer(3),
    HaveRemotePrAnswer(4),
    Closed(5);

    @Getter
    private int index;

    SignalingState(int index) {
        this.index = index;
    }

    public static SignalingState getByIndex(int index) {
        return values()[index];
    }
}
