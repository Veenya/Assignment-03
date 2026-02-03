#include <Arduino.h>
#include "config.h"
#include "kernel/MsgService.h"
#include "kernel/Scheduler.h"
#include "model/CommunicationCenter.h"
#include "model/HWPlatform.h"
#include "model/UserPanel.h"
#include "tasks/CommunicationTask.h"
#include "tasks/InputTask.h"

Scheduler scheduler;
HWPlatform* pHWPlatform;
UserPanel* pUserPanel;
TankSystem *pTankSystem;
CommunicationCenter* pCommunicationCenter;

//todo tolgiere obrobrio
Button* button;
PotentiometerImpl* pot;

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

    // TODO: mettici HWPlatform dioc ane
    Task* pInputTask = new InputTask(pTankSystem, pUserPanel, button, pot); // TankSystem* pTankSystem, UserPanel* pUserPanel, Button* button, PotentiometerImpl* pot
    pInputTask->init();

    scheduler.addTask(pCommunicationTask);
    scheduler.addTask(pInputTask);
}

void loop() {
    scheduler.schedule();
}
