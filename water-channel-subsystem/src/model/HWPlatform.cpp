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

/// ------------------ TEST ------------------

void HWPlatform::test() {
    // Requisiti: pLcd inizializzato (chiama init() prima)
    static bool prevPressed = false;

    // Leggi bottone (se ButtonImpl usa INPUT_PULLUP: premuto = LOW)
    bool nowPressed = pButton->isPressed();   // se non esiste, vedi nota sotto

    // Edge detect: vero solo al fronte di salita (pressione)
    bool justPressed = (nowPressed && !prevPressed);
    prevPressed = nowPressed;

    // Leggi potenziometro
    int raw = pPotentiometer->position();     // tipicamente 0..1023
    int pct = map(raw, 0, 1023, 0, 100);

    // Stampa su Serial
    Serial.print("BTN=");
    Serial.print(nowPressed ? "1" : "0");
    Serial.print("  POT=");
    Serial.print(raw);
    Serial.print("  PCT=");
    Serial.println(pct);

    // Mostra su LCD
    pLcd->setCursor(0, 0);
    pLcd->print("BTN:");
    pLcd->print(nowPressed ? "ON " : "OFF");
    pLcd->print(" POT:");
    if (pct < 100) pLcd->print(" ");
    if (pct < 10)  pLcd->print(" ");
    pLcd->print(pct);
    pLcd->print("%");

    pLcd->setCursor(0, 1);
    if (justPressed) {
        pLcd->print("Pressed!        ");
    } else {
        pLcd->print("                ");
    }

    delay(100);
}