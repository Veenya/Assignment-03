#ifndef __POTENTIOMETER_IMPL__
#define __POTENTIOMETER_IMPL__

#include "Potentiometer.h"
#include "config.h"

class PotentiometerImpl : public Potentiometer {
public:
    PotentiometerImpl(int pin, int tolerance);
    void sync() override;
    long getLastSynchTime();
    unsigned int position();
    bool moved();

private:
    long lastTimeSync;
    int pin;
    int tolerance;
    int oldValue;
};

#endif