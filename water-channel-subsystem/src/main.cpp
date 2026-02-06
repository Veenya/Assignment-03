#include <Arduino.h>
#include "config.h"
#include "kernel/MsgService.h"
#include "kernel/Scheduler.h"
#include "model/CommunicationCenter.h"
#include "model/HWPlatform.h"
#include "model/UserPanel.h"
#include "tasks/CommunicationTask.h"
#include "tasks/PotentiometerTask.h"
#include "tasks/ControllerTask.h"

Scheduler scheduler;
HWPlatform* pHWPlatform;
UserPanel* pUserPanel;
Controller *pController;
CommunicationCenter* pCommunicationCenter;
ControllerTask* pControllerTask;
Task* pCommunicationTask;
PotentiometerTask* pPotentiometerTask;

void setup() {
    MsgService.init();
    scheduler.init(SCHEDULER_PERIOD);

    pHWPlatform = new HWPlatform();
    pHWPlatform->init();

    pUserPanel = new UserPanel(pHWPlatform);
    pUserPanel->init();

    pController = new Controller(pHWPlatform);
    pController->init();

    pCommunicationCenter = new CommunicationCenter(pController);
    pCommunicationCenter->init();

    pCommunicationTask = new CommunicationTask(pCommunicationCenter, pController);
    pCommunicationTask->init(COMMUNICATION_PERIOD);

    pControllerTask = new ControllerTask(pController); 
    pControllerTask->init(TANK_PERIOD);

    pPotentiometerTask = new PotentiometerTask(pController);
    pControllerTask->init(POTENTIOMETER_PERIOD);

    scheduler.addTask(pCommunicationTask);
    scheduler.addTask(pControllerTask);
}

void loop() {
    //scheduler.schedule();
}
