package bbm.webrtc.rtc4j.core;

import bbm.webrtc.rtc4j.model.LogLevel;
import java.nio.ByteBuffer;

/**
 * @author bbm
 */
public class Utils {

    static {
        RTC.log("RTC utils loaded", LogLevel.INFO.getIndex());
    }

    public static native int scaleImage(ByteBuffer source, ByteBuffer rgbBuffer, ByteBuffer target, int factorNumerator,
        int factorDenominator, int quality);
}
