package bbm.webrtc.rtc4j.model;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * @author bbm
 */
@AllArgsConstructor
@NoArgsConstructor
@Data
public class Turn{
    private String uri;
    private String username;
    private String password;
}
