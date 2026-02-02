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

void HWPlatform::test() {
    // TODO: opzionale
}

// --- getters ---

ServoMotor* HWPlatform::getValveMotor() {
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