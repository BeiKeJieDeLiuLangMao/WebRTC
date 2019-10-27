package bbm.webrtc.demo;

import com.corundumstudio.socketio.SocketIOServer;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.CommandLineRunner;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.scheduling.annotation.EnableScheduling;
import org.springframework.stereotype.Component;

/**
 * @author bbm
 */
@SpringBootApplication
@EnableScheduling
public class Application {

    public static void main(String[] args) {
        SpringApplication.run(Application.class, args);
    }

    @Component
    public static class ServerRunner implements CommandLineRunner {

        @Autowired
        private SocketIOServer server;

        @Override
        public void run(String... args) throws InterruptedException {
            while (true) {
                try {
                    server.start();
                    break;
                }catch (Exception e) {
                    Thread.sleep(1000);
                }
            }
        }

    }
}
