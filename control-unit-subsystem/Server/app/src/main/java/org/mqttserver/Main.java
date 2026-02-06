package org.mqttserver;

import io.vertx.core.Vertx;
import org.mqttserver.services.http.DataService;
import org.mqttserver.services.mqtt.RemoteBrokerClientImpl;

public class Main {

    public static void main(String[] args) {
        // 1) MQTT client (verso broker online)
        RemoteBrokerClientImpl mqttClient = new RemoteBrokerClientImpl(
                "broker.mqtt-dashboard.com",
                1883,
                "java-client-demo-123"
        );
        mqttClient.start();

        // 2) HTTP service (per la dashboard)
        Vertx vertx = Vertx.vertx();
        int httpPort = 8050;

        vertx.deployVerticle(new DataService(httpPort, mqttClient.getSystemController()));
    }
}
