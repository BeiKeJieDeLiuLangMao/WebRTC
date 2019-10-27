package bbm.webrtc.demo.handler;

import com.corundumstudio.socketio.SocketIOClient;
import com.corundumstudio.socketio.annotation.OnConnect;
import com.corundumstudio.socketio.annotation.OnDisconnect;
import java.util.Map;
import java.util.UUID;
import java.util.concurrent.ConcurrentHashMap;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Component;

/**
 * @author bbm
 */
@SuppressWarnings("unused")
@Component
@Slf4j
public class SessionHandler{

    private static final Map<UUID, SocketIOClient> CLIENT_MAP = new ConcurrentHashMap<>();

    @OnConnect
    public void onConnect(SocketIOClient client) {
        log.info("A client connected, {}", client.getSessionId());
        CLIENT_MAP.put(client.getSessionId(), client);
    }

    @OnDisconnect
    public void onDisconnect(SocketIOClient client) {
        log.info("A client disconnected, {}", client.getSessionId());
        CLIENT_MAP.remove(client.getSessionId(), client);
    }

    public void broadcast(String name, Object data) {
        CLIENT_MAP.forEach((uuid, client) -> client.sendEvent(name, data));
    }
}
