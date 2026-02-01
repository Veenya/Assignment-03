#include "CommunicationTask.h"
#include <Arduino.h>
#include "config.h"
#include "kernel/Logger.h"

CommunicationTask::CommunicationTask(CommunicationCenter* pCommunicationCenter, Controller* pController)
    : pCommunicationCenter(pCommunicationCenter), pController(pController) {
    setState(WaterState::Low);
}

void CommunicationTask::tick() {
    // 1) aggiorna informazioni dal canale seriale / remoto
    if (pCommunicationCenter) {
        pCommunicationCenter->sync();  // leggere eventuali messaggi in arrivo dal DRU
    }

    // 2) invia periodicamente lo stato corrente al DRU
    lastStateUpdate = 0;
    now = millis();

    if (pCommunicationCenter && (now - lastStateUpdate >= STATE_UPDATE_PERIOD)) {
        lastStateUpdate = now;
        pCommunicationCenter->notifyNewState();
    }
}

void CommunicationTask::setState(WaterState waterState) {
    this->waterState = waterState;
    waterStateTimestamp = millis();
    justEntered = true;
}

long CommunicationTask::elapsedTimeInState() {
    return millis() - waterStateTimestamp;
}
