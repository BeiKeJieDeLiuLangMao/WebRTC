package bbm.webrtc.rtc4j.model;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * @author bbm
 */
@Data
@AllArgsConstructor
@NoArgsConstructor
public class SessionDescription {
    private String type;
    private String sdp;
}
