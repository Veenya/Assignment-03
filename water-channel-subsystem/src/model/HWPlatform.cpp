#include "HWPlatform.h"
#include <Arduino.h>
#include "devices/button/ButtonImpl.h"
#include "devices/servo_motor/servo_motor_impl.h"
#include "devices/potentiometer/PotentiometerImpl.h"

void wakeUp() {}

HWPlatform::HWPlatform() {
    pMotor = new ServoMotorImpl(SERVO_PIN);
    pLcd = new LiquidCrystal_I2C(LCD_ADDR, 16, 2);
    pButton = new ButtonImpl(BUTTON_PIN);
    pPotentiometer = new PotentiometerImpl(POTENTIOMETER_PIN, POTENTIOMETER_TOLERANCE);
    Serial.println("HWPlatform istanziata");
}

void HWPlatform::init() {
    pLcd->init();
    pLcd->backlight();
    pLcd->clear();

    if (pMotor) {
        pMotor->motorOn();
        pMotor->setPosition(0);
    }

    Serial.println("HWPlatform inizializzata");
}

// --- getters ---

ServoMotorImpl* HWPlatform::getValveMotor() {
    return pMotor;
}

ButtonImpl* HWPlatform::getToggleButton() {
    return pButton;
}

PotentiometerImpl* HWPlatform::getPotentiometer() {
    return pPotentiometer;
}

LiquidCrystal_I2C* HWPlatform::getLcd() {
    return pLcd;
}

/// ------------------ TEST ------------------

void HWPlatform::test() {
    static int mode;              // 0=AUTO, 1=MANUAL (persists)
    static int lastReading = HIGH;    // because INPUT_PULLUP: released=HIGH
    static unsigned long lastDebounceTime = 0;
    const unsigned long debounceMs = 40;

    // Read raw button (pressed = LOW)
    int reading = digitalRead(BUTTON_PIN);

    // Debounce: detect stable change
    if (reading != lastReading) {
        lastDebounceTime = millis();
        lastReading = reading;
    }

    // When stable for debounceMs and currently pressed -> toggle once
    static int stableState = HIGH;
    if ((millis() - lastDebounceTime) > debounceMs) {
        if (reading != stableState) {
            stableState = reading;

            if (stableState == LOW) {     // pressed event
                mode = 1 - mode;          // toggle
            }
        }
    }

    // Pot (your position() should already be 0..100)
    int pct = (int)pPotentiometer->position();
    pMotor->setPosition(pct);

    // Serial
    Serial.print("MODE=");
    Serial.print(mode == 0 ? "AUTO" : "MANUAL");
    Serial.print("  POT=");
    Serial.print(pct);
    Serial.println("%");

    // LCD
    pLcd->setCursor(0, 0);
    pLcd->print("MODE:");
    pLcd->print(mode == 0 ? "AUTO   " : "MANUAL ");

    pLcd->setCursor(0, 1);
    pLcd->print("POT:");
    if (pct < 100) pLcd->print(" ");
    if (pct < 10)  pLcd->print(" ");
    pLcd->print(pct);
    pLcd->print("%   ");

    
}
