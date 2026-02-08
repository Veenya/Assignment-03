#ifndef __COMMUNICATION_TASK__
#define __COMMUNICATION_TASK__

#include "kernel/Task.h"
#include "model/CommunicationCenter.h"
#include "model/Controller.h"
#include "model/States.h"

class CommunicationTask : public Task {
public:
    CommunicationTask(CommunicationCenter* pCommunicationCenter);
    void tick();

private:
    unsigned long lastStateUpdate;
    unsigned long now;
    CommunicationCenter* pCommunicationCenter;
};

#endif
