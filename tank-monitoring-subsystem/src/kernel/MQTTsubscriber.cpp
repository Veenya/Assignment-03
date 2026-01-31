#include "kernel/MQTTsubscriber.h"

MQTTsubscriber::MQTTsubscriber(const char* server, const char* client_id)
    : MQTTservice(server, client_id) {
    // Se MQTTservice non inizializza mqttClient, fallo qui
    // mqttClient = PubSubClient();  ← se serve (di solito fatto in base)
}

void MQTTsubscriber::begin() {
    mqttClient.setServer(mqtt_server, MQTT_PORT);  // usa membri ereditati

    mqttClient.subscribe(FREQ_TOPIC);

    mqttClient.setCallback([this](char* topic, byte* payload, unsigned int length) {
        this->handleMessage(topic, payload, length);
    });

    Serial.print("Sottoscritto a: ");
    Serial.println(FREQ_TOPIC);
}

void MQTTsubscriber::loop() {
    mqttClient.loop();
}

boolean MQTTsubscriber::isConnected() {
    return mqttClient.connected();
}

int MQTTsubscriber::getSavedFrequency() const {
    return saved_frequency;
}

void MQTTsubscriber::handleMessage(char* topic, byte* payload, unsigned int length) {
    Serial.print("Messaggio ricevuto su [");
    Serial.print(topic);
    Serial.print("] → ");

    // Stampa payload per debug
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    // Se è il topic della frequenza → processa JSON o valore semplice
    if (strcmp(topic, FREQ_TOPIC) == 0) {
        processFrequencyPayload(payload, length);
    }
}

void MQTTsubscriber::processFrequencyPayload(byte* payload, unsigned int length) {
    // Tenta prima parsing JSON (il tuo caso originale)
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload, length);

    if (error == DeserializationError::Ok) {
        // JSON valido → leggi campo "freq"
        int freq = doc[FREQUENCY_FIELD];
        if (freq > 0) {
            saved_frequency = freq;
            Serial.print("Nuova frequenza da JSON: ");
            Serial.println(freq);
            return;
        }
    } else {
        Serial.print("JSON non valido: ");
        Serial.println(error.c_str());
    }

    // Fallback: tratta payload come numero semplice (molto comune per test)
    String msg(reinterpret_cast<char*>(payload), length);
    msg.trim();

    long val = msg.toInt();
    if (val > 0) {
        saved_frequency = (int)val;
        Serial.print("Nuova frequenza da testo semplice: ");
        Serial.println(val);
    } else {
        Serial.println("Payload non valido né come JSON né come numero");
    }
}