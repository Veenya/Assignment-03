#ifndef __POTENTIOMETER_IMPL__
#define __POTENTIOMETER_IMPL__

#include "Potentiometer.h"

class PotentiometerImpl : public Potentiometer {
public:
    PotentiometerImpl(int pin, int tolerance);

    // Ritorna posizione normalizzata (in base ai define nel config.h)
    unsigned int getPosition();

    // True se il pot è cambiato più di "tolerance"
    bool moved();

private:
    int pin;
    int tolerance;
    int oldValue;
};

#endif
