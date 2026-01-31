#include "HWPlatform.h"
#include <Arduino.h>
// #include "devices/led/Led.h"
// #include "devices/proximity_sensor/Sonar.h"

// void wakeUp() {}

HWPlatform::HWPlatform() {
    // pDdd = new Sonar(SONAR_ECHO_PIN, SONAR_TRIG_PIN, MAX_TIME_US);
    // pL1 = new Led(L1_PIN);
    // pL2 = new Led(L2_PIN);
    pWiFiConnection = new WiFiConnection(WIFI_SSID, WIFI_PASSWORD);
    pMQTTsubscriber = new MQTTsubscriber(DEFAULT_MQTT_SERVER, SUBSCRIBER_CLIENT_ID);
    pMQTTpublisher = new MQTTpublisher(DEFAULT_MQTT_SERVER, PUBLISHER_CLIENT_ID);
    Serial.println("HWPlatform istanziata");
}

void HWPlatform::init() {
    Serial.println("\n=== WiFiConnection ===\n");
    pWiFiConnection->setup_wifi();

    if (false) {  // TODO cancellare finiti i debug
        Serial.println("Test risoluzione DNS...");
        IPAddress addr;
        if (WiFi.hostByName("www.google.com", addr)) {
            Serial.print("DNS OK per google → IP: ");
            Serial.println(addr);
        } else {
            Serial.println("DNS FALLITO anche per google.com");
        }
    }

    pMQTTpublisher->connect();
    pMQTTsubscriber->connect();
    pMQTTsubscriber->begin();  // ← sostituisce subscribeJSON

    // In loop() o nella task subscriber
    pMQTTsubscriber->loop();

    // MQTT Publisher
    Serial.println("Connessione MQTT publisher...");
    if (pMQTTpublisher->connected()) {
        Serial.println("Publisher connesso");
    } else {
        Serial.println("Publisher NON connesso");
    }

    // MQTT Subscriber
    Serial.println("Connessione MQTT subscriber...");
    if (pMQTTsubscriber->connected()) {
        Serial.println("Subscriber connesso");
    } else {
        Serial.println("Subscriber NON connesso");
    }

    Serial.println("HWPlatform inizializzata");
}

Led* HWPlatform::getL1() {
    return this->pL1;
}

Led* HWPlatform::getL2() {
    return this->pL2;
}

Sonar* HWPlatform::getDDD() {
    return this->pDdd;
}

WiFiConnection* HWPlatform::getWiFiConnection() {
    return this->pWiFiConnection;
};
MQTTsubscriber* HWPlatform::getMQTTsubscriber() {
    return this->pMQTTsubscriber;
};
MQTTpublisher* HWPlatform::getMQTTpublisher() {
    return this->pMQTTpublisher;
};