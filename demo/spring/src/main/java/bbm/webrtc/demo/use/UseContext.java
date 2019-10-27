package bbm.webrtc.demo.use;

import bbm.webrtc.rtc4j.core.DataChannel;
import bbm.webrtc.rtc4j.core.PeerConnection;
import bbm.webrtc.rtc4j.core.RTC;
import com.corundumstudio.socketio.SocketIOClient;
import java.util.Objects;
import java.util.concurrent.locks.ReentrantLock;
import lombok.Getter;
import lombok.Setter;
import lombok.extern.slf4j.Slf4j;

/**
 * @author bbm
 */
@SuppressWarnings("unused")
@Slf4j
@Getter
public class UseContext {
    private final SocketIOClient client;
    private final ReentrantLock lock = new ReentrantLock(true);
    @Setter
    private DataChannel dataChannel;
    private RTC rtc;
    private volatile PeerConnection peerConnection;

    public UseContext(SocketIOClient client) {
        this.client = client;
    }

    public void executeInLock(Runnable runnable) {
        lock.lock();
        try {
            runnable.run();
        } finally {
            lock.unlock();
        }
    }

    public void setRtc(RTC rtc) {
        if (!Objects.isNull(this.peerConnection)) {
            this.peerConnection.close();
            this.peerConnection = null;
        }
        if (!Objects.isNull(this.rtc)) {
            this.rtc.close();
            this.rtc = null;
        }
        this.rtc = rtc;
    }

    public void setPeerConnection(PeerConnection peerConnection) {
        if (!Objects.isNull(this.peerConnection)) {
            this.peerConnection.close();
            this.peerConnection = null;
        }
        this.peerConnection = peerConnection;
    }

    public void releaseRtc() {
        if (!Objects.isNull(this.peerConnection)) {
            this.peerConnection.stopTransport();
            this.peerConnection.close();
            this.peerConnection = null;
        }
        if (!Objects.isNull(this.rtc)) {
            this.rtc.close();
            this.rtc = null;
        }
    }
}
