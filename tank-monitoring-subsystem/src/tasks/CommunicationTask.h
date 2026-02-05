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

    unsigned long lastStateUpdateWater;
    unsigned long lastStateUpdateMQTT;

    unsigned long now;

    void setWaterState(WaterState waterState);
    void setMQTTState(MQTTState mqttState);

    long elapsedTimeInWaterState();
    long elapsedTimeInMQTTState();

    long waterStateTimestamp;
    long mqttStateTimestamp;
    long mqttLastConnected;

    bool justEnteredWater;
    bool justEnteredMQTT;

    WaterState waterState;
    MQTTState mqttState;

    CommunicationCenter* pCommunicationCenter;
    Controller* pController;
};

#endif
