package bbm.webrtc.rtc4j.model;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;
import lombok.experimental.Accessors;

/**
 * @author bbm
 */
@Data
@AllArgsConstructor
@NoArgsConstructor
@Accessors(chain = true)
public class DataChannelConfig {

    /**
     * Deprecated. Reliability is assumed, and channel will be unreliable if
     * maxRetransmitTime or MaxRetransmits is set.
     */
    private Boolean reliable = false;

    /**
     * True if ordered delivery is required.
     */
    private Boolean ordered = true;

    /**
     * The max period of time in milliseconds in which retransmissions will be
     * sent. After this time, no more retransmissions will be sent. -1 if unset.
     *
     * Cannot be set along with |maxRetransmits|.
     */
    private Integer maxRetransmitTime = -1;

    /**
     * The max number of retransmissions. -1 if unset.
     *
     * Cannot be set along with |maxRetransmitTime|.
     */
    private Integer maxRetransmits = -1;

    /**
     * This is set by the application and opaque to the WebRTC implementation.
     */
    private String protocol;

    /**
     * True if the channel has been externally negotiated and we do not send an
     * in-band signalling in the form of an "open" message. If this is true, |id|
     * below must be set; otherwise it should be unset and will be negotiated
     * in-band.
     */
    private Boolean negotiated = false;

    /**
     * The stream id, or SID, for SCTP data channels. -1 if unset (see above).
     */
    private Integer id = -1;
}
