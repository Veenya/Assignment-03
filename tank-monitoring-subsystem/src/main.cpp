#include <Arduino.h>
#include "config.h"
#include "kernel/MQTTpublisher.h"
#include "kernel/MQTTsubscriber.h"
#include "kernel/WiFiConnection.h"
// #include "kernel/Logger.h"
// #include "kernel/MsgService.h"
#include "kernel/Scheduler.h"
// #include "model/CommunicationCenter.h"
// #include "model/HWPlatform.h"
// #include "tasks/CommunicationTask.h"
// #include "tasks/SonarTask.h"

// Scheduler scheduler;
// HWPlatform* pHWPlatform;
WiFiConnection* pWiFiConnection;
MQTTsubscriber* pMQTTsubscriber;
MQTTpublisher* pMQTTpublisher;

// // UserPanel* pUserPanel;
// // Hangar* pHangar;
// // CommunicationCenter* pCommunicationCenter;

unsigned long lastPublish = 0;
const unsigned long PUBLISH_INTERVAL = 5000;  // ogni 5 secondi

// ---------------------------------------------------------------
// Callback per quando arriva un messaggio sul topic sottoscritto
// ---------------------------------------------------------------
void onMessageReceived(char* topic, uint8_t* payload, unsigned int length) {
    Serial.print("Messaggio su topic: ");
    Serial.print(topic);
    Serial.print(" → ");

    // Stampa il payload come stringa (se è testo/JSON)
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    // Se ti aspetti un numero intero (es. "15000" sul topic FREQ_TOPIC)
    String msg = "";
    for (unsigned int i = 0; i < length; i++) {
        msg += (char)payload[i];
    }

    long val = msg.toInt();
    if (val > 0) {
        Serial.print("→ Valore intero ricevuto: ");
        Serial.println(val);
        // Qui potresti aggiornare la tua variabile frequency = val;
    } else {
        Serial.println("Payload non è un numero valido");
    }
}

void setup() {
    // MsgService.init();
    // scheduler.init(SCHEDULER_PERIOD);
    // Logger.log(":::::: Tank Monitoring Subsystem ::::::");
    // pHWPlatform = new HWPlatform();
    // pHWPlatform->init();

    pWiFiConnection = new WiFiConnection(WIFI_SSID, WIFI_PASSWORD);
    pMQTTsubscriber = new MQTTsubscriber(DEFAULT_MQTT_SERVER, SUBSCRIBER_CLIENT_ID);
    pMQTTpublisher = new MQTTpublisher(DEFAULT_MQTT_SERVER, PUBLISHER_CLIENT_ID);

    // pUserPanel = new UserPanel(pHWPlatform);
    // pUserPanel->init();

    // pHangar = new Hangar(pHWPlatform);
    // pHangar->init();

    // pCommunicationCenter = new CommunicationCenter(pHangar);
    // pCommunicationCenter->init();

    // Task* pCommunicationTask = new CommunicationTask(pCommunicationCenter, pHangar);
    // pCommunicationTask->init(COMMUNICATION_PERIOD);

    // Task* pHangarTask = new HangarTask(pCommunicationCenter, pHangar, pUserPanel);
    // pHangarTask->init(DOOR_TASK);

    // TODO Task* pSonar = new SonarTask(pHangar, pUserPanel);
    // pSonar->init(SONAR_TASK);

    // TODO scheduler.addTask(pSonarTask);

    Serial.begin(ESP_BAUD);
    delay(200);

    // WiFi
    Serial.println("\n=== WiFiConnection ===\n");
    pWiFiConnection->setup_wifi();
    if (pWiFiConnection->isConnected()) {
        Serial.print("WiFi OK - IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("Connessione fallita → controlla credenziali e segnale");
    }

    Serial.println("Test risoluzione DNS...");
    IPAddress addr;
    if (WiFi.hostByName("www.google.com", addr)) {
        Serial.print("DNS OK per google → IP: ");
        Serial.println(addr);
    } else {
        Serial.println("DNS FALLITO anche per google.com");
    }

    Serial.print("\n\nDEBUG CONFIG: MQTT_SERVER = [");
    Serial.print(DEFAULT_MQTT_SERVER);
    Serial.println("]\n");

    pMQTTpublisher->connect();
    pMQTTsubscriber->connect();
    pMQTTsubscriber->subscribeJSON(FREQ_TOPIC);

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

    // Sottoscrizione + callback
    pMQTTsubscriber->subscribeJSON(FREQ_TOPIC);       // il tuo topic
    pMQTTsubscriber->setCallback(onMessageReceived);  // ← fondamentale!

    Serial.println("\nSetup completato. Inizio publish ogni 5s e listen...");
}

void loop() {
    // scheduler.schedule();
    pMQTTpublisher->loop();   // o publisher->client.loop() se il wrapper lo espone
    pMQTTsubscriber->loop();  // idem

    // Esempio di publish periodico
    unsigned long now = millis();
    if (now - lastPublish >= PUBLISH_INTERVAL) {
        lastPublish = now;

        if (pMQTTpublisher->connected()) {
            const char* testTopic = FREQ_TOPIC;
            const char* testMsg = "Ciao dal test ESP32 - " __DATE__ " " __TIME__;

            Serial.print("Publish su ");
            Serial.print(testTopic);
            Serial.print(" → ");
            Serial.println(testMsg);

            pMQTTpublisher->publish(testTopic, testMsg);  // usa il metodo semplice
            // Oppure publisher->publishJSON(...) se preferisci il tuo formato JSON
        } else {
            Serial.println("Publisher non connesso → skip publish");
        }
    }

    delay(10);  // piccolo respiro (non bloccante)
}
