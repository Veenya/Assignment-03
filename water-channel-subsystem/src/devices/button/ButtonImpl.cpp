#include "ButtonImpl.h"
#include "Arduino.h"

ButtonImpl::ButtonImpl(int pin) {
    this->pin = pin;
    pinMode(pin, INPUT);
    // sync();
    clicked = false;
}

bool ButtonImpl::isPressed() {
    return pressed;
}

bool ButtonImpl::isClickedAndReset() { 
    Serial.print("Clicked: ");
    Serial.println(clicked);
    bool wasClicked = clicked;
    clicked = false;
    return wasClicked;
    
}

void ButtonImpl::sync() {
    Serial.println("ButtonImpl sync");
    bool wasPressed = pressed;
    pressed = digitalRead(pin) == HIGH;
    updateSyncTime(millis());

    if (pressed) {
        clicked = false;
        return;
    }
    clicked = wasPressed;
}