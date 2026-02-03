#include <Arduino.h>
#include "config.h"
#include "kernel/MsgService.h"
#include "kernel/Scheduler.h"
#include "model/CommunicationCenter.h"
#include "model/HWPlatform.h"
#include "model/UserPanel.h"
#include "tasks/CommunicationTask.h"
#include "tasks/TankTask.h"

Scheduler scheduler;
HWPlatform* pHWPlatform;
UserPanel* pUserPanel;
TankSystem *pTankSystem;
CommunicationCenter* pCommunicationCenter;
TankTask* pTankTask;

void setup() {
    MsgService.init();
    scheduler.init(SCHEDULER_PERIOD);

    pHWPlatform = new HWPlatform();
    pHWPlatform->init();

    pUserPanel = new UserPanel(pHWPlatform);
    pUserPanel->init();

    pTankSystem = new TankSystem(pHWPlatform);
    pTankSystem->init();

    pCommunicationCenter = new CommunicationCenter(pTankSystem);
    pCommunicationCenter->init();

    Task* pCommunicationTask = new CommunicationTask(pCommunicationCenter, pTankSystem);
    pCommunicationTask->init(COMMUNICATION_PERIOD);

    Task* pTankTask = new TankTask(pHWPlatform); 
    pTankTask->init(TANK_PERIOD);

    scheduler.addTask(pCommunicationTask);
    scheduler.addTask(pTankTask);
}

void loop() {
    //scheduler.schedule();
    pHWPlatform->test();
}
