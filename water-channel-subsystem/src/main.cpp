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
#include "tasks/ButtonTask.h"

Scheduler scheduler;
HWPlatform* pHWPlatform;
UserPanel* pUserPanel;
Controller *pController;
CommunicationCenter* pCommunicationCenter;
Task* pControllerTask;
Task* pCommunicationTask;
Task* pPotentiometerTask;
Task* pButtonTask;

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

    pCommunicationTask = new CommunicationTask(pCommunicationCenter);
    pCommunicationTask->init(COMMUNICATION_PERIOD);

    pPotentiometerTask = new PotentiometerTask(pController);
    pPotentiometerTask->init(POTENTIOMETER_PERIOD);
    
    pControllerTask = new ControllerTask(pController, pCommunicationCenter, pUserPanel); 
    pControllerTask->init(CONTROLLER_PERIOD);

    pButtonTask = new ButtonTask(pController);
    pButtonTask->init(BUTTON_PERIOD);


    scheduler.addTask(pCommunicationTask);
    scheduler.addTask(pPotentiometerTask);
    scheduler.addTask(pControllerTask);
    scheduler.addTask(pButtonTask);
}

void loop() {
    MsgService.update();
    scheduler.schedule();
}
