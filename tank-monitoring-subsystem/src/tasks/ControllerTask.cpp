#include "ControllerTask.h"
#include <Arduino.h>
#include "config.h"
// #include "kernel/Logger.h"

ControllerTask::ControllerTask(Controller* pController, CommunicationCenter* pCommunicationCenter)
    : pController(pController), pCommunicationCenter(pCommunicationCenter) {
    pHW = pController->getHWPlatform();
}

void ControllerTask::tick() {
    this->waterState = pController->getWaterState();
    manageLeds();
}

void ControllerTask::manageLeds() {
    if (pCommunicationCenter->getMQTTState() == MQTTState::CONNECTED) {
        pHW->getL1()->switchOn();
        pHW->getL2()->switchOff();
    } else {
        pHW->getL2()->switchOn();
        pHW->getL1()->switchOff();
    }
}

// long ControllerTask::elapsedTimeInState() {
//     return millis() - stateTimestamp;
// }