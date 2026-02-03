#ifndef __TANK_SYSTEM__
#define __TANK_SYSTEM__

#include "HWPlatform.h"
#include "UserPanel.h"
#include "States.h"
#include "config.h"

/*
 * Tank Monitoring / Water Channel subsystem model.
 *
 * - TMS (ESP): measures WL via sonar and publishes to CUS via MQTT.
 * - WCS (Arduino): controls valve (servo) and provides local operator panel
 *   (button + potentiometer + LCD).
 *
 * This class is meant to represent the WCS-side "system state" and I/O:
 * - mode: AUTOMATIC / MANUAL
 * - connectivity: CONNECTED / UNCONNECTED (derived from CUS commands / heartbeat)
 * - valve opening: 0..100%
 * - operator inputs: button toggle, potentiometer value
 * - outputs: servo position, LCD text
 */

class TankSystem {
public:
    explicit TankSystem(HWPlatform* hw);

    void init();
    void sync(); // refresh inputs/outputs (read button/pot, update lcd/servo if needed)

    /* --------- Mode & connectivity --------- */
    void setMode(SystemMode mode);
    SystemMode getMode();

    void setConnectivity(ConnectivityState state);
    ConnectivityState getConnectivity();

    bool isManual();
    bool isUnconnected();

    /* --------- Water level (from CUS -> WCS for display) ---------
     * On WCS you may show WL on LCD if CUS sends it.
     * If not needed, you can remove these.
     */
    void setWaterLevel(float wl);
    float getWaterLevel();

    /* --------- Valve control --------- */
    // Commanded valve opening in percentage [0..100]
    void setValveOpening(int percent);
    int getValveOpening();

    // Apply the commanded opening to the servo (0°..90°)
    void applyValveToServo();

    /* --------- Operator inputs (WCS local panel) --------- */
    // Button toggles MANUAL/AUTOMATIC (edge detection is usually in FSM/task)
    bool isModeButtonPressed();

    // Potentiometer reading mapped to 0..100 (used only in MANUAL)
    int readManualValveFromPot();

    /* --------- Outputs --------- */
    // LCD shows: valve%, mode (AUTO/MANUAL) or UNCONNECTED
    void updateDisplay();

    HWPlatform* pHW;

    // state
    SystemMode mode;                 // AUTOMATIC / MANUAL
    ConnectivityState connectivity;  // CONNECTED / UNCONNECTED
    float waterLevel;                // last WL known (optional, for LCD)
    int valveOpening;                // 0..100 commanded/current

    // cached input readings (optional)
    bool lastButtonState;

    // helpers
    int clampPercent(int v);
    int percentToServoAngle(int percent);
};

#endif
