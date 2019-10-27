package bbm.webrtc.rtc4j.core;

import bbm.webrtc.rtc4j.utils.SystemUtils;
import java.nio.ByteBuffer;

/**
 * @author bbm
 */
public class ImageUtils {

    static {
        SystemUtils.loadNativeLibrary();
    }

    public static native int scaleImage(ByteBuffer source, ByteBuffer rgbBuffer, ByteBuffer target, int factorNumerator, int factorDenominator, int quality);
}
