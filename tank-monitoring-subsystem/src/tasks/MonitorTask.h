#ifndef __MONITOR_TASK__
#define __MONITOR_TASK__

#include "kernel/Task.h"
// #include "model/CommunicationCenter.h"
#include "model/Monitor.h"
#include "model/States.h"


class MonitorTask : public Task {
public:
    MonitorTask(Monitor* pMonitor);
    void tick();

private:
    // void setDoorState(DoorState state);
    // long elapsedTimeInState();

    bool droneInRange;

    // MonitorState monitorState;
    long stateTimestamp;

    Monitor* pMonitor;
    WaterState waterState;
    MQTTState mqttState;
};

#endif
