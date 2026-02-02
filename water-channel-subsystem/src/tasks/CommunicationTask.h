#ifndef __COMMUNICATION_TASK__
#define __COMMUNICATION_TASK__

#include "kernel/Task.h"
#include "model/CommunicationCenter.h"
#include "model/TankSystem.h"
#include "model/States.h"

/*
 * Runs every 20–50 ms.
    Responsibilities:
    Read serial messages from CUS:
        MODE,AUTO / MODE,MANUAL
        VALVE,0..100
        optional WL,xx.x (for LCD)
        optional PING
    Mark CONNECTED when any valid message arrives
    If no valid message for T2 → set UNCONNECTED
    Periodically send status to CUS:
       STATE,<mode>,<conn>,<valve>,<wl>
    Why: this implements the “WCS communicates via serial line with CUS” + supports UNCONNECTED.
 */
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
