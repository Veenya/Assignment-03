#ifndef __CONTROLLER__
#define __CONTROLLER__

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

class Controller {
public:
    explicit Controller(HWPlatform* hw);
    void init();

    /* --------- Mode & connectivity --------- */
    void setSystemState(SystemState systemState);
    SystemState getSystemState();
    ConnectivityState getConnectivity();

    bool isManualRemote();
    bool isManualLocal();
    bool isUnconnected();
    void moveMotor(int angle);
    void syncButton();
    bool buttonCheckAndConsumeClick();

    /* --------- Water level (from CUS -> WCS for display) ---------
     * On WCS you may show WL on LCD if CUS sends it.
     * If not needed, you can remove these.
     */
    void setWaterLevel(float waterLevel);
    float getWaterLevel();

    /* --------- Valve control --------- */
    // Commanded valve opening in percentage [0..100]
    void setValveOpening(int valveOpening);

    // Apply the commanded opening to the servo (0°..90°)
    void applyValveToServo();

    int getValveOpening();

    void setPotentiometerPosition(int potentiometerPosition); 
    float getPotentiometerPosition();
    HWPlatform* getHWPlatform();
    ConnectivityState getConnectivityState();
    void setConnectivityState(ConnectivityState connectivityState);
    int clampPercent(int v);
    int percentToServoAngle(int percent);

private:
    HWPlatform* pHW;
    SystemState systemState; 
    ConnectivityState connectivityState;
    ServoMotorImpl* pServo;
    ButtonImpl* pBtn;
    
    float waterLevel;                // last WL known (optional, for LCD)
    float potentiometerPosition;
    int valveOpening;
    bool lastButtonState;
};

#endif
