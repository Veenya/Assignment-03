#ifndef __CONTROLLER_TASK__
#define __CONTROLLER_TASK__

#include "kernel/Task.h"
#include "model/CommunicationCenter.h"
#include "model/Controller.h"
#include "model/States.h"

class ControllerTask : public Task {
public:
    ControllerTask(Controller* pController, CommunicationCenter* pCommunicationCenter);
    void tick();

private:
    void manageLeds();
    void manageWaterLevel();
    Controller* pController;
    HWPlatform* pHW;
    WaterState waterState;
    MQTTState mqttState;
    CommunicationCenter* pCommunicationCenter;
    long elapsedTimeInState();
    long stateTimestamp;
    bool justEntered;
    void setWaterState(WaterState waterState);
};

#endif
