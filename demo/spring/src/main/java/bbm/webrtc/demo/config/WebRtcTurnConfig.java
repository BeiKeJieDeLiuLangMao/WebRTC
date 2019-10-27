package bbm.webrtc.demo.config;

import bbm.webrtc.rtc4j.model.Turn;
import java.util.List;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.stereotype.Component;

/**
 * @author bbm
 */
@Component
@ConfigurationProperties(prefix = "webrtc.ice")
@AllArgsConstructor
@NoArgsConstructor
@Data
public class WebRtcTurnConfig {
    private List<Turn> turns;

    public Turn[] getTurnServers() {
        return turns.toArray(new Turn[0]);
    }
}
