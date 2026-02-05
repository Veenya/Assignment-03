#include "CommunicationTask.h"
#include <Arduino.h>
#include "config.h"
#include "kernel/Logger.h"

CommunicationTask::CommunicationTask(CommunicationCenter* pCommunicationCenter, Controller* pController)
    : pCommunicationCenter(pCommunicationCenter), pController(pController) {
    setWaterState(WaterState::Low);
    mqttLastConnected = 0;
}

void CommunicationTask::tick() {
    // invia periodicamente lo stato corrente
    now = millis();

    if (pCommunicationCenter && (now - lastStateUpdate >= STATE_UPDATE_PERIOD) && pCommunicationCenter->checkMQTTConnection()) {
        lastStateUpdate = now;
        pCommunicationCenter->notifyNewState();
    }
}

void CommunicationTask::setWaterState(WaterState waterState) {
    this->waterState = waterState;
    waterStateTimestamp = millis();
    justEnteredWater = true;
}

void CommunicationTask::setMQTTState(MQTTState mqttState) {
    // se l'ultima conn e' stata 5 se fa, KO
    // else reset e ok
    if (mqttState == MQTTState::KO) {
        // start timer: aspetta 5 sec
        if (mqttLastConnected > 5000) {
            // Turn on red LED, turn off other led..?
            Serial.printf("ERRRO");
        }

    } else {
        // Con OK
        // reset timer
        mqttLastConnected = now;
    }
}

long CommunicationTask::elapsedTimeInWaterState() {
    return millis() - waterStateTimestamp;
}
