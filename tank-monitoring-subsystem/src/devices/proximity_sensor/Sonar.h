#ifndef __SONAR__
#define __SONAR__

#include "ProximitySensor.h"
#include "config.h"

class Sonar : public ProximitySensor {
public:
    Sonar(int echoPin, int trigPin, long maxTime);
    float getDistance() override;
    void setTemperature(float temp);

private:
    float temperature;
    int echoPin, trigPin;
    long timeOut;
    long distance;
    long duration;
};

#endif