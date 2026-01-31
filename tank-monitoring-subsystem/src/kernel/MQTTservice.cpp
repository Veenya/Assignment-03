#include "kernel/MQTTservice.h"
#include <IPAddress.h>   // ← Aggiungi questo include (Arduino/ESP32 lo ha già)

void MQTTservice::connect() {
    IPAddress brokerIP(192, 168, 1, 98);

    Serial.print("DEBUG HARDCODE: IP prima setServer = ");
    Serial.println(brokerIP);

    mqttClient.setServer(brokerIP, 1883);

    Serial.print("DEBUG: IP dopo setServer = ");
    Serial.println(mqttClient.serverIP());

    if (mqttClient.serverIP() == IPAddress(0,0,0,0)) {
        Serial.println("ERRORE: serverIP è 0.0.0.0 → setServer non ha preso l'IP!");
    }

    Serial.println("Connecting...");
    bool ok = mqttClient.connect(mqtt_client_id);
    if (ok) {
        Serial.println("Connected!");
    } else {
        Serial.print("Failed, rc=");
        Serial.println(mqttClient.state());
    }
}

void MQTTservice::loop() {
    mqttClient.loop();
}

bool MQTTservice::connected() {
    return mqttClient.connected();
}