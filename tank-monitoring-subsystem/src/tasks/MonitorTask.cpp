#include "MonitorTask.h"
#include <Arduino.h>
#include "config.h"
// #include "kernel/Logger.h"

MonitorTask::MonitorTask(Monitor* pMonitor) : pMonitor(pMonitor) {
    // pMonitor->setDoorState(DoorState::CLOSED);
}

void MonitorTask::tick() {
    this->waterState = pMonitor->getWaterState();
    this->mqttState = pMonitor->getMQTTState();

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

// long MonitorTask::elapsedTimeInState() {
//     return millis() - stateTimestamp;
// }
