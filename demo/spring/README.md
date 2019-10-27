# Fesaid Spring Demo

## Profiles

* empty: Normal usage
* agent: Run as remote device agent, you could edit [profile](./src/main/resources/application-agent.yml) to control proxy port and which device is proxy-able
* debug: Run as remote device debug client, you could edit [profile](./src/main/resources/application-debug.yml) to control adb server's host and port

# Project setup
```bash
 # Normal usage
 mvn spring-boot:run
 # Run as remote device agent
 mvn spring-boot:run -Dspring.profiles.active=agent
 # Run as remote device debug
 mvn spring-boot:run -Dspring.profiles.active=debug
```