#include "HWPlatform.h"
#include <Arduino.h>
#include "devices/button/ButtonImpl.h"
#include "devices/servo_motor/servo_motor_impl.h"
#include "devices/potentiometer/PotentiometerImpl.h"

void wakeUp() {}

HWPlatform::HWPlatform() {
    pDoorMotor = new ServoMotorImpl(SERVO_PIN);
    pLcd = new LiquidCrystal_I2C(LCD_ADDR, 16, 2);
    pResetButton = new ButtonImpl(BUTTON_PIN);
    pPotentiometer = new PotentiometerImpl(POTENTIOMETER_PIN, POTENTIOMETER_TOLERANCE);
    Serial.println("HWPlatform istanziata");
}

void HWPlatform::init() {
    pLcd->init();
    pLcd->backlight();
    Serial.println("HWPlatform inizializzata");
}


