// // TODO ciclicamente misura il livello dell'acqua

// #ifndef __SONAR_TASK__
// #define __SONAR_TASK__

// #include "kernel/Task.h"
// #include "model/CommunicationCenter.h"
// #include "model/HWPlatform.h"
// #include "model/Hangar.h" // TODO: cambiare
// #include "model/States.h"
// #include "model/UserPanel.h" // TODO: cambiare

// class SonarTask : public Task {
// public:
//     SonarTask(Hangar* pHangar, UserPanel* pUserPanel); //TODO: change Hangar in Monitor
//     void tick();

// private:
//     void setState(WaterState s); //Change
//     long elapsedTimeInState();
//     //void log(const String& msg);

//     long stateTimestamp;
//     bool justEntered;
//     WaterState state;

//     UserPanel* pUserPanel;
//     Hangar* pHangar;
// };

// #endif