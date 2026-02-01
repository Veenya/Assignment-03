#include <Arduino.h>
#include "config.h"
#include "kernel/Scheduler.h"
#include "model/HWPlatform.h"
#include "model/Controller.h"
#include "tasks/ControllerTask.h"
#include "tasks/SonarTask.h"
#include "model/CommunicationCenter.h"
#include "tasks/CommunicationTask.h"

// #include "kernel/Logger.h"
// #include "kernel/MsgService.h"

HWPlatform* pHWPlatform;
Scheduler scheduler;
Controller* pController;
CommunicationCenter* pCommunicationCenter;

// UserPanel* pUserPanel;

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

    pCommunicationCenter = new CommunicationCenter(pController);
    pCommunicationCenter->init();

    Task* pCommunicationTask = new CommunicationTask(pCommunicationCenter, pController);
    pCommunicationTask->init(COMMUNICATION_PERIOD);

    Task* pSonarTask = new SonarTask(pController);
    pSonarTask->init(SONAR_TASK);

    Task* pControllerTask = new ControllerTask(pController, pCommunicationCenter);
    pControllerTask->init(CONTROLLER_TASK);


    // scheduler.addTask(pSonarTask);
    // scheduler.addTask(pControllerTask);
    

    Serial.println("\n=== SETUP OK ===\n");
}

void loop() {
    // scheduler.schedule();

    

    // delay(10);  // piccolo respiro (non bloccante)
}
