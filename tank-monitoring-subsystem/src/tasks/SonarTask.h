// TODO ciclicamente misura il livello dell'acqua

#ifndef __SONAR_TASK__
#define __SONAR_TASK__

#include "kernel/Task.h"
#include "model/CommunicationCenter.h"
#include "model/HWPlatform.h"
#include "model/Monitor.h"
#include "model/States.h"
// #include "model/UserPanel.h"

class SonarTask : public Task {
public:
    SonarTask(Monitor* pMonitor);
    void tick();

private:
    void setState(WaterState s); //Change
    long elapsedTimeInState();
    //void log(const String& msg);

    long stateTimestamp;
    bool justEntered;
    WaterState state;

    // UserPanel* pUserPanel;
    Monitor* pMonitor;
};

#endif