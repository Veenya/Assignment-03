#ifndef __COMMUNICATION_TASK__
#define __COMMUNICATION_TASK__

#include "kernel/Task.h"
#include "model/CommunicationCenter.h"
#include "model/Controller.h"
#include "model/States.h"

class CommunicationTask : public Task {
public:
    CommunicationTask(CommunicationCenter* pCommunicationCenter, Controller* pController);
    void tick();

private:
    unsigned long lastStateUpdate;
    unsigned long now;
    void setState(SystemState state);
    long elapsedTimeInState();

    SystemState state;
    long stateTimestamp;
    bool justEntered;

    CommunicationCenter* pCommunicationCenter;
    Controller* pController;
};

#endif
