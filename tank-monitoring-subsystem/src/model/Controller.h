// TODO controlla lo stato e di conseguenza fa cose: controlla i led,...
// Controllers the rainwater level in the tank
// Based on that it turns on the LEDs

#ifndef __CONTROLLER__
#define __CONTROLLER__

#include "HWPlatform.h"
#include "States.h"
#include "config.h"

class Controller {
public:
    Controller(HWPlatform* hw);

    void init();
    void sync();

    /* --- Controller State ---*/
    void checkNetwork();
    void checkData();
    void setControllerState();

    /* --- Water State ---*/
    void setWaterState(WaterState state);
    WaterState getWaterState();
    float getDistance();
    MQTTState getMQTTState();

    HWPlatform* getHWPlatform();
/*
The rainwater level are sampled at frequency F and sent to the CUS subsystem.
When the system is working correctly (network ok, sending data ok) 
the green led is on and the red is off; 
otherwise – in the case of network problems – the red led should be on and the green led off.
*/


    void setL1On();
    void setL2On();
    void setL1Off();
    void setL2Off();

private:
    HWPlatform* pHW;
    WaterState waterState;
    MQTTState mqttState;


    bool L1isOn;
    bool L2isOn;
    bool doorOpen;
    float lastDistance;
    float currentTemp;
    float lastTemperature;
    bool alarmRaised;
    void manageLeds();

};

#endif
