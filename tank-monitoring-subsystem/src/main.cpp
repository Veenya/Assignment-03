#include <Arduino.h>
#include "config.h"
#include "model/HWPlatform.h"
// #include "kernel/MQTTpublisher.h"
// #include "kernel/MQTTsubscriber.h"
// #include "kernel/WiFiConnection.h"
// #include "kernel/Logger.h"
// #include "kernel/MsgService.h"
// #include "kernel/Scheduler.h"
// #include "model/CommunicationCenter.h"
// #include "tasks/CommunicationTask.h"
// #include "tasks/SonarTask.h"

// Scheduler scheduler;
HWPlatform* pHWPlatform;


// UserPanel* pUserPanel;
// Hangar* pHangar;
// CommunicationCenter* pCommunicationCenter;



void setup() {
    // MsgService.init();
    // scheduler.init(SCHEDULER_PERIOD);
    // Logger.log(":::::: Tank Monitoring Subsystem ::::::");
    Serial.begin(ESP_BAUD);
    delay(200);
    Serial.println("\n=== TEST WiFi + MQTT MINIMALE ===\n");

    pHWPlatform = new HWPlatform();
    pHWPlatform->init();


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

    

    Serial.println("\n=== SETUP OK ===\n");
}

void loop() {
    // scheduler.schedule();
    // pMQTTpublisher->loop();   // o publisher->client.loop() se il wrapper lo espone
    // pMQTTsubscriber->loop();  // idem

    // // Esempio di publish periodico
    // unsigned long now = millis();
    // if (now - lastPublish >= PUBLISH_INTERVAL) {
    //     lastPublish = now;

    //     if (pMQTTpublisher->connected()) {
    //         const char* testTopic = FREQ_TOPIC;
    //         const char* testMsg = "Ciao dal test ESP32 - " __DATE__ " " __TIME__;

    //         Serial.print("Publish su ");
    //         Serial.print(testTopic);
    //         Serial.print(" → ");
    //         Serial.println(testMsg);

    //         pMQTTpublisher->publish(testTopic, testMsg);  // usa il metodo semplice
    //         // Oppure publisher->publishJSON(...) se preferisci il tuo formato JSON
    //     } else {
    //         Serial.println("Publisher non connesso → skip publish");
    //     }
    // }

    // delay(10);  // piccolo respiro (non bloccante)
}
