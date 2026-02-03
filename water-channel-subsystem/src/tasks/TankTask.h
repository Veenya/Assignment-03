#ifndef __TANK_TASK__
#define __TANK_TASK__

#include "model/HWPlatform.h"
#include "model/States.h"
#include "config.h"

/*
 * WCS-side system state and I/O:
 * - mode: AUTOMATIC / MANUAL
 * - connectivity: CONNECTED / UNCONNECTED
 * - valve opening: 0..100%
 * - operator inputs: button toggle, potentiometer value
 * - outputs: servo position, LCD text
 */
class TankTask {
public:
    explicit TankTask(HWPlatform* hw);

    void init();

    /* --------- Mode & connectivity --------- */
    void setMode(SystemMode mode);
    SystemMode getMode() const;

    void toggleMode();

    void setConnectivity(ConnectivityState state);
    ConnectivityState getConnectivity() const;

    bool isManual() const;
    bool isUnconnected() const;

    /* --------- Water level (optional for LCD) --------- */
    void setWaterLevel(float wl);
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

private:
    HWPlatform* pHW;

    // state
    SystemMode mode; 
    ConnectivityState connectivity;
    float waterLevel;
    int valveOpening;

    // cached input readings
    bool lastButtonState;

    // helpers
    int clampPercent(int v) const;
    int percentToServoAngle(int percent) const;
};

#endif
