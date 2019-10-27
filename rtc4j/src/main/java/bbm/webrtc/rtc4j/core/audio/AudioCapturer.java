package bbm.webrtc.rtc4j.core.audio;

import java.io.Closeable;
import java.nio.ByteBuffer;

/**
 * @author bbm
 */
public interface AudioCapturer extends Closeable {
    /**
     * Get capturer sample rate
     * @return Sample rate
     */
    int samplingFrequency();

    /**
     * Get audio data direct byte buffer
     *
     * @param size Expecting data size
     * @return Audio data in little endian encode
     */
    ByteBuffer capture(int size);
}
