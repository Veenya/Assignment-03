#include "CommunicationCenter.h"
#include <Arduino.h>
// #include "kernel/Logger.h"
// #include "kernel/MsgService.h"

CommunicationCenter::CommunicationCenter(Controller* pController) : pController(pController) {
    pWiFiConnection = new WiFiConnection(WIFI_SSID, WIFI_PASSWORD);
    pMQTTsubscriber = new MQTTsubscriber(DEFAULT_MQTT_SERVER, SUBSCRIBER_CLIENT_ID);
    pMQTTpublisher = new MQTTpublisher(DEFAULT_MQTT_SERVER, PUBLISHER_CLIENT_ID);
    MQTTState mqttState = MQTTState::KO;
    Serial.println("Communication Center istanziato");
}

void CommunicationCenter::init() {
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

    // pMQTTpublisher->connect();
    // pMQTTsubscriber->connect();
    // pMQTTsubscriber->begin();

    // pMQTTpublisher->loop();
    // pMQTTsubscriber->loop();

    // MQTT Publisher
    Serial.println("Connessione MQTT publisher e subscriber ...");
    checkMQTTConnection();
    Serial.println("Communication Center inizializzato");
}

// checkMQTTConnection
bool CommunicationCenter::checkMQTTConnection() {
    if (mqttState == MQTTState::KO) {
        pMQTTpublisher->connect();
        pMQTTsubscriber->connect();
        pMQTTsubscriber->begin();

        pMQTTpublisher->loop();
        pMQTTsubscriber->loop();
    }

    if (pMQTTpublisher->connected() && pMQTTsubscriber->connected()) {
        mqttState = MQTTState::CONNECTED;
        Serial.println("Publisher e subscriber connessi");
        return true;
    } else {
        mqttState = MQTTState::KO;
        if (!pMQTTpublisher->connected()) {
            Serial.println("Errore Connessione Publisher");
        } 
        if (!pMQTTsubscriber->connected()) {
            Serial.println("Errore Connessione Subscriber");
        }
        return false;
    }
}

void CommunicationCenter::notifyNewState() {
    String message;
    this->waterState = pController->getWaterState();
    if (waterState == WaterState::Low) {
        message = "Low";
    } else if (waterState == WaterState::Medium) {
        message = "Mid";
    } else if (waterState == WaterState::High) {
        message = "High";
    }

    if (pMQTTpublisher->connected()) {
        // const char* message = "Ciao dal test ESP32 - " __DATE__ " " __TIME__;
        //TODO: togliere
        mqttState = MQTTState::CONNECTED;
        pController->setMQTTState(MQTTState::CONNECTED);
        Serial.print("Publish su " FREQ_TOPIC " → ");
        Serial.println(message);

        pMQTTpublisher->publish(FREQ_TOPIC, message.c_str());  // metodo semplice
        //c_str() restituisce un puntatore a const char*
        // publisher->publishJSON(...) altrimenti JSON
    } else {
        mqttState = MQTTState::KO;
        pController->setMQTTState(MQTTState::KO);
        
        Serial.println("Publisher non connesso → skip publish");
    }
}

WiFiConnection* CommunicationCenter::getWiFiConnection() {
    return this->pWiFiConnection;
};
MQTTsubscriber* CommunicationCenter::getMQTTsubscriber() {
    return this->pMQTTsubscriber;
};
MQTTpublisher* CommunicationCenter::getMQTTpublisher() {
    return this->pMQTTpublisher;
};

MQTTState CommunicationCenter::getMQTTState() {
    return this->mqttState;
}