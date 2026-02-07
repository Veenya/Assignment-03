package org.mqttserver.services.mqtt;

import io.netty.handler.codec.mqtt.MqttQoS;
import io.vertx.core.Vertx;
import io.vertx.core.buffer.Buffer;
import io.vertx.mqtt.MqttClient;
import io.vertx.mqtt.MqttClientOptions;
import io.vertx.mqtt.messages.MqttPublishMessage;
import org.mqttserver.policy.SystemController;
import org.mqttserver.policy.SystemControllerImpl;
import org.mqttserver.presentation.JSONUtils;
import org.mqttserver.presentation.MessageFromSensor;
//import org.mqttserver.presentation.MessageToSensor;

public class RemoteBrokerClientImpl {

    // Topic (coerenti e riusabili)
    private static final String TOPIC_WL = "/sensor/wl";

    private final SystemController systemController = new SystemControllerImpl();

    private final Vertx vertx;        // motore asincrono che gestisce eventi (connessioni, messaggi, callback)
    private final MqttClient client;  // client MQTT vero e proprio

    private final String host;
    private final int port;
    private final String clientId;

    public RemoteBrokerClientImpl(String host, int port, String clientId) {
        this.host = host;
        this.port = port;
        this.clientId = clientId;

        this.vertx = Vertx.vertx();

        MqttClientOptions options = new MqttClientOptions()
                .setClientId(clientId)                              // identifica il client sul broker
                .setKeepAliveInterval(30000)     // vertx manda automaticamente pacchetti keep-alive per mantenere viva la connessione
                .setAutoKeepAlive(true)            
                .setCleanSession(true);                // sessione "pulita"

        this.client = MqttClient.create(vertx, options);
    }

    /**
     * Avvia la connessione al broker online e si sottoscrive ai topic necessari.
     * connessione al broker + subscribe + handler messaggi
     */
    public void start() {
        client.connect(port, host, conn -> {
            if (conn.failed()) {
                System.err.println("MQTT connect FAILED: " + conn.cause().getMessage());
                return;
            }

            System.out.println("Connected to MQTT broker " + host + ":" + port + " as " + clientId);

            // Subscribe ai messaggi del sensore (WL)
            client.subscribe(TOPIC_WL, MqttQoS.AT_MOST_ONCE.value(), sub -> {
                if (sub.succeeded()) {
                    System.out.println("Subscribed to " + TOPIC_WL);
                } else {
                    System.err.println("Subscribe FAILED on " + TOPIC_WL + ": " + sub.cause().getMessage());
                }
            });

            // Handler dei publish in arrivo
            client.publishHandler(this::handleIncomingPublish);
        });

        client.closeHandler(v -> System.out.println("MQTT connection closed"));
        client.exceptionHandler(err -> System.err.println("MQTT error: " + err.getMessage()));
    }

    /**
     * Stoppa la connessione e chiude Vertx.
     */
    public void stop() {
        try {
            client.disconnect(d -> {
                if (d.succeeded()) {
                    System.out.println("Disconnected from broker");
                } else {
                    System.err.println("Disconnect FAILED: " + d.cause().getMessage());
                }
                vertx.close();
            });
        } catch (Exception e) {
            vertx.close();
        }
    }

    private void handleIncomingPublish(MqttPublishMessage message) {
        String topic = message.topicName();
        String payloadStr = safePayloadToString(message.payload());

        System.out.println("MQTT IN topic=" + topic + " payload=" + payloadStr);

        // Qui gestisci SOLO quello che ti interessa (WL)
        if (!TOPIC_WL.equals(topic)) {
            return;
        }

        try {
            // Parse del JSON che arriva dal sensore
            MessageFromSensor parsed = JSONUtils.jsonToObject(payloadStr, MessageFromSensor.class);   // {"WL": 12.3}
            float wl = parsed.getWL();

            System.out.println("Received WL on " + topic + ": " + wl + " payload=" + payloadStr);

            // Aggiorna sempre lo stato
            updateSystem(wl); 

            // Se in manuale: non inviare comandi automatici al sensore
            if (systemController.getIsManual()) {
                System.out.println("Manual mode ON: system updated, no automatic command sent.");
                return;
            }

        } catch (Exception e) {
            System.err.println("Failed to handle message on " + topic + ": " + e.getMessage());
        }
    }

    private void updateSystem(float wl) {
        systemController.setWL(wl);
    }

    private static String safePayloadToString(Buffer payload) {
        if (payload == null) return "";
        return payload.toString(); // se serve encoding specifico: payload.toString("UTF-8")
    }

    public SystemController getSystemController() {
        return systemController;
    }
}
