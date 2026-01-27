#include "Led.h"
#include "Arduino.h"
#include "config.h"

Led::Led(int pin) {
    this->pin = pin;
    pinMode(pin, OUTPUT);
    state = false;
    switchOff();
}

void Led::switchOn() {
    state = true;
    digitalWrite(pin, HIGH);
}

void Led::switchOff() {
    state = false;
    digitalWrite(pin, LOW);
}

void Led::toggle() {
    if (state)
        switchOff();
    else
        switchOn();
}
