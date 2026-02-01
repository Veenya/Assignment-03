#ifndef __CONTROLLER_TASK__
#define __CONTROLLER_TASK__

#include "kernel/Task.h"
// #include "model/CommunicationCenter.h"
#include "model/Controller.h"
#include "model/States.h"


class ControllerTask : public Task {
public:
    ControllerTask(Controller* pController);
    void tick();

private:
    // void setDoorState(DoorState state);
    // long elapsedTimeInState();

    bool droneInRange;

    // ControllerState cotrollerState;
    long stateTimestamp;

    Controller* pController;
    WaterState waterState;
    MQTTState mqttState;
};

#endif
