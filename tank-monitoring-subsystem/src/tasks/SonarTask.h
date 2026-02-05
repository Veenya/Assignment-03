#ifndef __SONAR_TASK__
#define __SONAR_TASK__

#include "kernel/Task.h"
#include "model/CommunicationCenter.h"
#include "model/Controller.h"
#include "model/HWPlatform.h"
#include "model/States.h"

class SonarTask : public Task {
public:
    SonarTask(Controller* pController);
    void tick();
    void setWaterState(WaterState waterState);
    

private:
    long elapsedTimeInState();
    long stateTimestamp;
    bool justEntered;
    WaterState state;
    Controller* pController;
};

#endif