package bbm.webrtc.rtc4j.model;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * @author bbm
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class IceCandidate {
    String sdpMid;
    Integer sdpMLineIndex;
    String candidate;
}
