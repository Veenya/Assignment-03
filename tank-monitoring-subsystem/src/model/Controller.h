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
    void setWaterState(WaterState state);
    void setWaterDistance(float distance);
    float getWaterDistance();
    float getDistance();
    void setL1On();
    void setL2On();
    void setL1Off();
    void setL2Off();
    WaterState getWaterState();
    MQTTState getMQTTState();
    HWPlatform* getHWPlatform();
    
    private:
    HWPlatform* pHW;
    WaterState waterState;
    MQTTState mqttState;
    bool L1isOn;
    bool L2isOn;
    bool doorOpen;
    float waterDistance;

};

#endif
