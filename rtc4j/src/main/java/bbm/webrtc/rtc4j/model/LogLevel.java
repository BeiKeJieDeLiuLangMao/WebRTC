package bbm.webrtc.rtc4j.model;

import lombok.Getter;

/**
 * @author bbm
 */
public enum LogLevel {

    /**
     * JNI log level enum
     */
    INFO(0),
    DEBUG(1),
    WARNING(2),
    ERROR(3);

    @Getter
    private int index;

    LogLevel(int index) {
        this.index = index;
    }

    public static LogLevel getByIndex(int index) {
        return values()[index];
    }
}
