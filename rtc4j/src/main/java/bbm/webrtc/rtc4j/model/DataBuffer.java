package bbm.webrtc.rtc4j.model;

import lombok.Data;

import java.nio.ByteBuffer;

/**
 * @author bbm
 */
@Data
public class DataBuffer {
    ByteBuffer data;
    Boolean binary;

    public DataBuffer(byte[] data, boolean binary) {
        this.data = ByteBuffer.allocateDirect(data.length).put(data);
        this.binary = binary;
    }

    public DataBuffer(ByteBuffer data) {
        this.data = data;
        this.binary = true;
    }
}