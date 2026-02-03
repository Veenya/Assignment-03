#ifndef __POTENTIOMETER_IMPL__
#define __POTENTIOMETER_IMPL__

#include "Potentiometer.h"

class PotentiometerImpl : public Potentiometer {
public:
    PotentiometerImpl(int pin, int tolerance);
    virtual ~PotentiometerImpl();

    void sync() override;

    // Ritorna posizione normalizzata (in base ai define nel config.h)
    unsigned int position();

    // True se il pot è cambiato più di "tolerance"
    bool moved();

private:
    int pin;
    int tolerance;
    int oldValue;
};

#endif
