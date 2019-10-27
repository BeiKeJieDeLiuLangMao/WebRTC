package bbm.webrtc.rtc4j.core;

import bbm.webrtc.rtc4j.core.audio.AudioCapturer;
import bbm.webrtc.rtc4j.core.observer.PeerConnectionObserver;
import bbm.webrtc.rtc4j.core.video.VideoCapturer;
import bbm.webrtc.rtc4j.model.LogLevel;
import bbm.webrtc.rtc4j.model.Turn;
import bbm.webrtc.rtc4j.utils.SystemUtils;
import lombok.Getter;
import lombok.extern.slf4j.Slf4j;

import java.io.Closeable;
import java.io.IOException;
import java.util.function.Function;

/**
 * @author bbm
 */
@Slf4j
public class RTC implements Closeable {

    protected Long nativeObject;
    private AudioCapturer audioCapturer;
    private VideoCapturer videoCapturer;
    @Getter
    private boolean openHardwareAccelerate;

    static {
        SystemUtils.loadNativeLibrary();
    }

    public RTC(AudioCapturer audioCapturer, String whitePrivateIpPrefix,int minPort, int maxPort){
        nativeObject = createNativeObject(audioCapturer, whitePrivateIpPrefix, minPort, maxPort, "");
        this.audioCapturer = audioCapturer;
    }

    public RTC(AudioCapturer audioCapturer, String whitePrivateIpPrefix,int minPort, int maxPort, String key) {
        nativeObject = createNativeObject(audioCapturer, whitePrivateIpPrefix, minPort, maxPort, key);
        this.audioCapturer = audioCapturer;
    }

    public RTC(AudioCapturer audioCapturer, VideoCapturer videoCapturer, String whitePrivateIpPrefix, int minPort,
        int maxPort, boolean openHardwareAccelerate) {
        nativeObject = createNativeObject(audioCapturer, videoCapturer, whitePrivateIpPrefix, minPort, maxPort, openHardwareAccelerate, "");
        this.audioCapturer = audioCapturer;
        this.videoCapturer = videoCapturer;
        this.openHardwareAccelerate = openHardwareAccelerate;
    }

    public RTC(AudioCapturer audioCapturer, VideoCapturer videoCapturer, String whitePrivateIpPrefix, int minPort,
               int maxPort, boolean openHardwareAccelerate, String key) {
        nativeObject = createNativeObject(audioCapturer, videoCapturer, whitePrivateIpPrefix, minPort, maxPort, openHardwareAccelerate, key);
        this.audioCapturer = audioCapturer;
        this.videoCapturer = videoCapturer;
        this.openHardwareAccelerate = openHardwareAccelerate;
    }

    public RTC(String audioCardMatchString, String whitePrivateIpPrefix,int minPort, int maxPort) {
        nativeObject = createNativeObject(audioCardMatchString, whitePrivateIpPrefix, minPort, maxPort, "");
    }

    public RTC(String audioCardMatchString, String whitePrivateIpPrefix,int minPort, int maxPort, String key) {
        nativeObject = createNativeObject(audioCardMatchString, whitePrivateIpPrefix, minPort, maxPort, key);
    }

    public static void log(String content, int level) {
        switch(LogLevel.getByIndex(level)) {
            case INFO:
                log.info("{}", content);
                break;
            case DEBUG:
                log.debug("{}", content);
                break;
            case WARNING:
                log.warn("{}", content);
                break;
            case ERROR:
                log.error("{}", content);
                break;
            default:
                //Not support, do nothing.
                break;
        }
    }

    synchronized  void executeInRtcLock(Function<Void, Void> function) {
        nullPointCheck();
        function.apply(null);
    }

    public synchronized PeerConnection createPeerConnection(PeerConnectionObserver peerConnectionObserver, Turn[] turns, int maxBitRate) {
        nullPointCheck();
        return new PeerConnection(createNativePeerConnection(nativeObject, peerConnectionObserver, turns, maxBitRate), peerConnectionObserver, this);
    }

    @Override
    public synchronized void close() {
        if (nativeObject != null) {
            free(nativeObject);
            nativeObject = null;
        }
        if (videoCapturer != null) {
            try {
                videoCapturer.close();
            } catch (IOException e) {
                log.warn("Close video capturer failed, {}", e);
            }
        }
        if (audioCapturer != null) {
            try {
                audioCapturer.close();
            } catch (IOException e) {
                log.warn("Close audio capturer failed, {}", e);
            }
        }
    }

    private void nullPointCheck() {
        if (nativeObject == null) {
            throw new NullPointerException("Object has been destroyed");
        }
    }

    private native long createNativeObject(AudioCapturer audioCapturer, String whiteIpPrefix, int minPort, int maxPort, String key);

    private native long createNativeObject(AudioCapturer audioCapturer, VideoCapturer videoCapturer, String whiteIpPrefix,
                                           int minPort, int maxPort, boolean openHardwareAccelerate, String key);

    private native long createNativeObject(String audioCardMatchString, String whiteIpPrefix, int minPort, int maxPort, String key);

    private native long createNativePeerConnection(long address, PeerConnectionObserver peerConnectionObserver,
                                                   Turn[] turns, int maxBitRate);

    private native void free(long address);

}
