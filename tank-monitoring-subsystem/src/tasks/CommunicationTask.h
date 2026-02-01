#ifndef __COMMUNICATION_TASK__
#define __COMMUNICATION_TASK__

#include "kernel/Task.h"
#include "model/CommunicationCenter.h"
#include "model/Controller.h"
#include "model/States.h"

/*
 * Task che gestisce la comunicazione con il DRU (Drone Remote Unit).
 * - legge i comandi dal PC (take-off, landing, reset...)
 * - aggiorna lo stato del drone nell'hangar
 * - notifica al DRU lo stato corrente (drone, hangar, distanza)
 */
class CommunicationTask : public Task {
public:
    CommunicationTask(CommunicationCenter* pCommunicationCenter, Controller* pController);
    void tick();

private:
    unsigned long lastStateUpdate;
    unsigned long now;
    void setState(WaterState waterState);
    long elapsedTimeInState();

    WaterState waterState;
    MQTTState mqttState;
    long waterStateTimestamp;
    bool justEntered;

    CommunicationCenter* pCommunicationCenter;
    Controller* pController;
};

#endif
