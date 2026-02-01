#include "ControllerTask.h"
#include <Arduino.h>
#include "config.h"
// #include "kernel/Logger.h"

ControllerTask::ControllerTask(Controller* pController) : pController(pController) {
    // pController->setDoorState(DoorState::CLOSED);
}

void ControllerTask::tick() {
    this->waterState = pController->getWaterState();
    this->mqttState = pController->getMQTTState();

    if (mqttState == MQTTState::KO) {
        Serial.println("Problemi con l'MQTT"); // TODO espandere e migliorare
    } else if (waterState == WaterState::Low) {
        Serial.println("TUTTO OK"); // TODO espandere e migliorare
    } else if (waterState == WaterState::Medium) {
        Serial.println("TUTTO OK"); // TODO espandere e migliorare
    } else if (waterState == WaterState::High) {
        Serial.println("TUTTO OK"); // TODO espandere e migliorare
    } 
}

// long ControllerTask::elapsedTimeInState() {
//     return millis() - stateTimestamp;
// }
