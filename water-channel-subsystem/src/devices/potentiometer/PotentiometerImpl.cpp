#include "PotentiometerImpl.h"

#include "Arduino.h"

PotentiometerImpl::PotentiometerImpl(int pin, int tolerance)
    : pin(pin), tolerance(tolerance), oldValue(analogRead(pin)) {
    pinMode(pin, INPUT);
}

unsigned int PotentiometerImpl::position() {
    int val = map(analogRead(pin), MIN_ANALOG_READ, MAX_ANALOG_READ, MIN_POTENTIOMETER, MAX_POTENTIOMETER);
    if (val < MIN_POTENTIOMETER)
        val = MIN_POTENTIOMETER;
    if (val > MAX_POTENTIOMETER)
        val = MAX_POTENTIOMETER;
    return val;
}

bool PotentiometerImpl::moved() {
    unsigned int val = analogRead(pin);
    /*
        NOTE: this ensures correct subtraction operation between oldValue and val as they are
        unsigned integers and direct subtraction could lead to unexpected results if val is less than
        oldValue.
    */
    bool hasMoved = abs(static_cast<int>(oldValue) - static_cast<int>(val)) > tolerance;
    oldValue = val;
    return hasMoved;
}