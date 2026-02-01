#include "ControllerTask.h"
#include <Arduino.h>
#include "config.h"
// #include "kernel/Logger.h"

ControllerTask::ControllerTask(Controller* pController, CommunicationCenter* pCommunicationCenter)
    : pController(pController), pCommunicationCenter(pCommunicationCenter) {
    // pController->setDoorState(DoorState::CLOSED);
    pHW = pController->getHWPlatform();
}

void ControllerTask::tick() {
    this->waterState = pController->getWaterState();
    this->mqttState = pController->getMQTTState();

    if (mqttState == MQTTState::KO) {
        Serial.println("Problemi con l'MQTT");  // TODO espandere e migliorare
    } 
}

// long ControllerTask::elapsedTimeInState() {
//     return millis() - stateTimestamp;
// }
void ControllerTask::manageLeds() {
    // Led1 Verde
    if (pCommunicationCenter->getMQTTState() == MQTTState::CONNECTED) {
        pHW->getL1()->switchOn();
        pHW->getL2()->switchOff();
    } else {
        pHW->getL2()->switchOn();
        pHW->getL1()->switchOff();
    }
}