#include "PotentiometerImpl.h"
#include <Arduino.h>
#include "config.h"

PotentiometerImpl::PotentiometerImpl(int pin, int tolerance)
    : pin(pin), tolerance(tolerance), oldValue(0) {

    pinMode(pin, INPUT);
    oldValue = analogRead(pin);
}

PotentiometerImpl::~PotentiometerImpl() {}

void PotentiometerImpl::sync() {
    // aggiorna timestamp (serve anche a evitare problemi di vtable)
    updateSyncTime(millis());
}

unsigned int PotentiometerImpl::position() {
    int raw = analogRead(pin);

    // Se nel tuo config.h NON esistono questi define, vedi nota sotto.
    int val = map(raw, MIN_ANALOG_READ, MAX_ANALOG_READ,
                  MIN_POTENTIOMETER, MAX_POTENTIOMETER);

    if (val < MIN_POTENTIOMETER) val = MIN_POTENTIOMETER;
    if (val > MAX_POTENTIOMETER) val = MAX_POTENTIOMETER;

    return (unsigned int)val;
}

bool PotentiometerImpl::moved() {
    int raw = analogRead(pin);
    bool hasMoved = abs(raw - oldValue) > tolerance;
    oldValue = raw;
    return hasMoved;
}
