#ifndef __LED__
#define __LED__

#include "Light.h"

class Led : public Light {
public:
    Led(int pin);
    void switchOn() override;
    void switchOff() override;

private:
    void toggle();
    int pin;
    bool state;
};

#endif