package bbm.webrtc.rtc4j.core.video;

import bbm.webrtc.rtc4j.model.VideoFrame;

import java.io.Closeable;

/**
 * @author bbm
 */
public interface VideoCapturer extends Closeable {

    int getWidth();

    int getHeight();

    int getFps();

    /**
     * Get video data direct byte buffer
     *
     * @return Image YUV I420 buffer
     */
    VideoFrame capture();
}
