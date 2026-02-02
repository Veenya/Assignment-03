package org.mqttserver.services.mqtt;
import io.vertx.mqtt.MqttServer;
import org.mqttserver.policy.SystemController;

public interface Broker {
    void initialize(MqttServer mqttServer);
    MqttServer getMqttServer();
    SystemController getSystemController();
}
