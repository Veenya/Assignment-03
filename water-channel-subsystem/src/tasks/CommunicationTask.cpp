#include "CommunicationTask.h"
#include <Arduino.h>
#include "config.h"

CommunicationTask::CommunicationTask(CommunicationCenter* pCommunicationCenter)
    : pCommunicationCenter(pCommunicationCenter) {}

void CommunicationTask::tick() {
    // Serial.println("CommunicationTask::tick");
    pCommunicationCenter->sync();            // leggere eventuali messaggi in arrivo dal DRU
    pCommunicationCenter->notifyNewState();  // invia messaggi al DRU
}
