package bbm.webrtc.rtc4j.core;

import bbm.webrtc.rtc4j.core.observer.DataChannelObserver;
import bbm.webrtc.rtc4j.core.observer.PeerConnectionObserver;
import bbm.webrtc.rtc4j.core.observer.SessionDescriptionObserver;
import bbm.webrtc.rtc4j.model.DataChannelConfig;
import bbm.webrtc.rtc4j.model.IceCandidate;
import bbm.webrtc.rtc4j.model.SessionDescription;
import bbm.webrtc.rtc4j.model.SignalingState;
import java.io.Closeable;
import lombok.Getter;
import lombok.extern.slf4j.Slf4j;

/**
 * @author bbm
 */
@Slf4j
public class PeerConnection implements Closeable {
    private Long nativeObject;
    private PeerConnectionObserver peerConnectionObserver;
    private RTC rtc;
    private boolean audioOpening = false;
    private boolean transporting = false;
    @Getter
    private boolean localSdpSet = false;

    PeerConnection(long nativeObject, PeerConnectionObserver peerConnectionObserver, RTC rtc) {
        this.nativeObject = nativeObject;
        this.peerConnectionObserver = peerConnectionObserver;
        this.rtc = rtc;
    }

    public synchronized void addIceCandidate(IceCandidate iceCandidate) throws Exception {
        nullPointCheck();
        String errorInfo = nativeAddIceCandidate(nativeObject, iceCandidate.getSdpMid(), iceCandidate.getSdpMLineIndex(),
                iceCandidate.getCandidate());
        if (errorInfo != null) {
            throw new Exception(errorInfo);
        }
    }

    public synchronized boolean isAudioOpening() {
        return audioOpening;
    }

    public synchronized boolean isInTransport() {
        return transporting;
    }

    public synchronized void openAudio() {
        if (audioOpening) {
            return;
        }
        nullPointCheck();
        rtc.executeInRtcLock(aVoid -> {
            nativeOpenAudio(nativeObject, rtc.nativeObject);
            return null;
        });
        audioOpening = true;
    }

    public synchronized void changeBitrate(int newBitrate) {
        nullPointCheck();
        nativeChangeBitrate(nativeObject, newBitrate);
    }

    public synchronized void closeAudio() {
        nullPointCheck();
        nativeCloseAudio(nativeObject);
        audioOpening = false;
    }

    public synchronized void startTransport() {
        if (transporting) {
            throw new RuntimeException("Transport is already start.");
        }
        nullPointCheck();
        rtc.executeInRtcLock(aVoid -> {
            nativeStartTransport(nativeObject, rtc.nativeObject);
            return null;
        });
        transporting = true;
    }

    public synchronized void stopTransport() {
        nullPointCheck();
        nativeStopTransport(nativeObject);
        transporting = false;
    }

    public synchronized void setLocalDescription(SessionDescription sdp) throws Exception {
        nullPointCheck();
        String errorInfo = nativeSetLocalDescription(nativeObject, sdp);
        if (errorInfo != null) {
            throw new Exception(errorInfo);
        }
        localSdpSet = true;
    }

    public synchronized void setRemoteDescription(SessionDescription sdp) throws Exception {
        nullPointCheck();
        String errorInfo = nativeSetRemoteDescription(nativeObject, sdp);
        if (errorInfo != null) {
            throw new Exception(errorInfo);
        }
    }

    public synchronized SignalingState getSignalingState() {
        nullPointCheck();
        return SignalingState.getByIndex(nativeGetSignalingState(nativeObject));
    }

    public synchronized void createAnswer(SessionDescriptionObserver sessionDescriptionObserver) {
        nullPointCheck();
        nativeCreateAnswer(nativeObject, sessionDescriptionObserver);
    }

    public synchronized void createOffer(SessionDescriptionObserver sessionDescriptionObserver) {
        nullPointCheck();
        nativeCreateOffer(nativeObject, sessionDescriptionObserver);
    }

    public synchronized DataChannel createDataChannel(String label, DataChannelConfig config, DataChannelObserver observer) {
        nullPointCheck();
        return new DataChannel(nativeCreateDataChannel(nativeObject, label, config, observer));
    }

    @Override
    public synchronized void close() {
        if (nativeObject != null) {
            if (audioOpening) {
                closeAudio();
            }
            free(nativeObject);
            nativeObject = null;
        }
    }

    private void nullPointCheck() {
        if (nativeObject == null) {
            throw new NullPointerException("Object has been destroyed");
        }
    }

    private native String nativeAddIceCandidate(long nativeObject, String sdpMid, int sdpMLineIndex, String candidate);

    private native String nativeSetLocalDescription(long nativeObject, SessionDescription sdp);

    private native String nativeSetRemoteDescription(long nativeObject, SessionDescription sdp);

    private native void nativeCreateAnswer(long nativeObject, SessionDescriptionObserver sessionDescriptionObserver);

    private native void nativeCreateOffer(long nativeObject, SessionDescriptionObserver sessionDescriptionObserver);

    private native Integer nativeGetSignalingState(long nativeObject);

    private native void nativeOpenAudio(long address, long rtcAddress);

    private native void nativeCloseAudio(long address);

    private native void nativeChangeBitrate(long address, int newBitrate);

    private native void nativeStartTransport(long address, long rtcAddress);

    private native void nativeStopTransport(long address);

    private native long nativeCreateDataChannel(long nativeObject, String label, DataChannelConfig dataChannelConfig,
                                                DataChannelObserver dataChannelObserver);

    private native void free(long address);

}
