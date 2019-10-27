package bbm.webrtc.rtc4j.model;

import lombok.AllArgsConstructor;

import java.nio.ByteBuffer;

/**
 * @author bbm
 */
@AllArgsConstructor
public class VideoFrame {
    int rotation;
    long timestamp;
    ByteBuffer dataBuffer;
    int length;

    public int getRotation() {
        return rotation;
    }

    public long getTimestamp() {
        return timestamp;
    }

    public int getLength() {
        return length;
    }

    public ByteBuffer getDataBuffer() {
        return dataBuffer;
    }
}
