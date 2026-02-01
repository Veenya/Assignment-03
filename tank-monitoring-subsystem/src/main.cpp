#include <Arduino.h>
#include "config.h"
#include "kernel/Scheduler.h"
#include "model/HWPlatform.h"
#include "model/Controller.h"
#include "tasks/ControllerTask.h"
#include "tasks/SonarTask.h"

// #include "kernel/Logger.h"
// #include "kernel/MsgService.h"
// #include "model/CommunicationCenter.h"
// #include "tasks/CommunicationTask.h"

HWPlatform* pHWPlatform;
Scheduler scheduler;
Controller* pController;

// UserPanel* pUserPanel;
// CommunicationCenter* pCommunicationCenter;

unsigned long lastPublish = 0;
const unsigned long PUBLISH_INTERVAL = 5000;  // ogni 5 secondi

void setup() {
    // MsgService.init();
    // Logger.log(":::::: Tank Controllering Subsystem ::::::");
    Serial.begin(ESP_BAUD);
    delay(200);
    Serial.println("\n=== TEST WiFi + MQTT MINIMALE ===\n");
    
    scheduler.init(SCHEDULER_PERIOD);
    pHWPlatform = new HWPlatform();
    pHWPlatform->init();


    // pUserPanel = new UserPanel(pHWPlatform);
    // pUserPanel->init();

    pController = new Controller(pHWPlatform);
    pController->init();

    // pCommunicationCenter = new CommunicationCenter(pHangar);
    // pCommunicationCenter->init();

    // Task* pCommunicationTask = new CommunicationTask(pCommunicationCenter, pHangar);
    // pCommunicationTask->init(COMMUNICATION_PERIOD);

    Task* pSonarTask = new SonarTask(pController);
    pSonarTask->init(SONAR_TASK);

    Task* pControllerTask = new ControllerTask(pController);
    pControllerTask->init(CONTROLLER_TASK);


    // scheduler.addTask(pSonarTask);
    // scheduler.addTask(pControllerTask);
    

    Serial.println("\n=== SETUP OK ===\n");
}

void loop() {
    // scheduler.schedule();

     // Esempio di publish periodico
    unsigned long now = millis();
    if (now - lastPublish >= PUBLISH_INTERVAL) {
        lastPublish = now;

        if (pHWPlatform->getMQTTpublisher()->connected()) {
            const char* testTopic = FREQ_TOPIC;
            const char* testMsg = "Ciao dal test ESP32 - " __DATE__ " " __TIME__;

            Serial.print("Publish su ");
            Serial.print(testTopic);
            Serial.print(" → ");
            Serial.println(testMsg);

            pHWPlatform->getMQTTpublisher()->publish(testTopic, testMsg);  // usa il metodo semplice
            // Oppure publisher->publishJSON(...) se preferisci il tuo formato JSON
        } else {
            Serial.println("Publisher non connesso → skip publish");
        }
    }

    // delay(10);  // piccolo respiro (non bloccante)
}
