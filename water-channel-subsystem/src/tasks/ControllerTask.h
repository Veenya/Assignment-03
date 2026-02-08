#ifndef __TANK_TASK__
#define __TANK_TASK__

#include "model/HWPlatform.h"
#include "model/States.h"
#include "model/CommunicationCenter.h"
#include "kernel/Task.h"
#include "config.h"

/*
 * WCS-side system state and I/O:
 * - mode: AUTOMATIC / MANUAL
 * - connectivity: CONNECTED / UNCONNECTED
 * - valve opening: 0..100%
 * - operator inputs: button toggle, potentiometer value
 * - outputs: servo position, LCD text
 */
class ControllerTask : public Task {
public:
    explicit ControllerTask(Controller* pController, CommunicationCenter* pCommunicationCenter, UserPanel* pUserPanel);
    void tick();

    /* --------- Mode & connectivity --------- */
    void setSystemState(SystemState systemMode);
    SystemState getSystemState() const;
    ConnectivityState getConnectivity() const;

    bool isManual() const;
    bool isUnconnected() const;

    /* --------- Water level (optional for LCD) --------- */
    void setWaterLevel(int waterLevel);
    float getWaterLevel() const;

    /* --------- Valve control --------- */
    void setValveOpening(int percent);
    int getValveOpening() const;

    void applyValveToServo();

    /* --------- Operator inputs --------- */
    bool isModeButtonPressed();      // edge detect
    int readManualValveFromPot();    // 0..100

    /* --------- Outputs --------- */
    void updateDisplay();
    void refreshOutputs();           // applyValveToServo + updateDisplay
    void setConnectivityState(ConnectivityState connectivityState);

private:
    HWPlatform* pHW;
    Controller* pController;
    CommunicationCenter* pCommunicationCenter;
    UserPanel* pUserPanel;
    SystemState systemState; 
    ConnectivityState connectivityState;
    ServoMotorImpl* pServo;
    PotentiometerImpl* pPotentiometer;
    ButtonImpl* pBtn;

    float waterLevel;
    int valveOpening;
    void checkSystemState();
    void manageValve();
};

#endif
