#ifndef __COMMUNICATION_TASK__
#define __COMMUNICATION_TASK__

#include "kernel/Task.h"
#include "model/CommunicationCenter.h"
#include "model/TankSystem.h"
#include "model/States.h"

class CommunicationTask : public Task {
public:
    CommunicationTask(CommunicationCenter* pCommunicationCenter, TankSystem* pTankSystem);
    void tick(); // override

private:
    unsigned long lastStateUpdate;
    unsigned long now;
    void setState(SystemMode state);
    long elapsedTimeInState();

    SystemMode state;
    long stateTimestamp;
    bool justEntered;

    CommunicationCenter* pCommunicationCenter;
    TankSystem* pTankSystem;
};

#endif
