#include <Arduino.h>
#include "WiFiConnection.h"
#include "config.h"
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

// // UserPanel* pUserPanel;
// // Hangar* pHangar;
// // CommunicationCenter* pCommunicationCenter;

void setup() {
    // MsgService.init();
    // scheduler.init(SCHEDULER_PERIOD);
    // Logger.log(":::::: Tank Monitoring Subsystem ::::::");
    // pHWPlatform = new HWPlatform();
    // pHWPlatform->init();

    pWiFiConnection = new WiFiConnection(WIFI_SSID, WIFI_PASSWORD);

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

    if (true) {
        Serial.begin(115200);
        delay(200);
        Serial.println("\n=== TEST WiFiConnection CLASS ===\n");

        pWiFiConnection->setup_wifi();

        if (pWiFiConnection->isConnected()) {
            Serial.println("Tutto ok");
        } else {
            Serial.println("Connessione fallita → controlla credenziali e segnale");
        }
    }
}

void loop() {
    // scheduler.schedule();
    if (true) {
        static unsigned long lastCheck = 0;

        if (millis() - lastCheck > 10000) {
            lastCheck = millis();

            if (pWiFiConnection->isConnected()) {
                Serial.print("[OK] Connesso - IP: ");
                Serial.println(pWiFiConnection->getLocalIP());
            } else {
                Serial.println("[!!] Connessione persa - ritento...");
                pWiFiConnection->setup_wifi();
            }
        }
    }
}

