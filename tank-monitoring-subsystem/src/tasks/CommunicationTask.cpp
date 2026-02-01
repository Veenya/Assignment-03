#include "CommunicationTask.h"
#include <Arduino.h>
#include "config.h"
#include "kernel/Logger.h"

CommunicationTask::CommunicationTask(CommunicationCenter* pCommunicationCenter, Controller* pController)
    : pCommunicationCenter(pCommunicationCenter), pController(pController) {
    setWaterState(WaterState::Low);
}

void CommunicationTask::tick() {
    // 2) invia periodicamente lo stato corrente
    lastStateUpdate = 0;
    now = millis();

    if (pCommunicationCenter && (now - lastStateUpdate >= STATE_UPDATE_PERIOD)) {
        lastStateUpdate = now;
        pCommunicationCenter->notifyNewState();
    }
}

void CommunicationTask::setWaterState(WaterState waterState) {
    this->waterState = waterState;
    waterStateTimestamp = millis();
    justEntered = true;
}

long CommunicationTask::elapsedTimeInState() {
    return millis() - waterStateTimestamp;
}
