package bbm.webrtc.demo.config;

import com.corundumstudio.socketio.Configuration;
import com.corundumstudio.socketio.SocketConfig;
import com.corundumstudio.socketio.SocketIOServer;
import com.corundumstudio.socketio.annotation.SpringAnnotationScanner;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;

/**
 * @author bbm
 */
@org.springframework.context.annotation.Configuration
public class SocketIoConfig {

    @Value("${socket.io.hostname}")
    private String hostname;
    @Value("${socket.io.port}")
    private int port;

    @Bean
    public SocketIOServer socketIOServer() {
        Configuration config = new Configuration();
        config.setHostname(hostname);
        config.setPort(port);
        config.setUpgradeTimeout(10000);
        config.setPingInterval(25000);
        config.setPingTimeout(60000);
        config.setBossThreads(1);
        config.setWorkerThreads(1);
        SocketConfig socketIoConfig = new SocketConfig();
        socketIoConfig.setReuseAddress(true);
        config.setSocketConfig(socketIoConfig);
        return new SocketIOServer(config);
    }

    /**
     * inject all event handler
     */
    @Bean
    public SpringAnnotationScanner springAnnotationScanner(SocketIOServer socketServer) {
        return new SpringAnnotationScanner(socketServer);
    }
}
