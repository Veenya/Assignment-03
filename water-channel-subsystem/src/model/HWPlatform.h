/* Contiene tutti gli oggetti device e usa i pin di config.h. */
#ifndef __HW_PLATFORM__
#define __HW_PLATFORM__

#include "config.h"  // pins

#include "devices/button/ButtonImpl.h"              // button
#include "devices/servo_motor/servo_motor.h"        // servo motor
#include "devices/potentiometer/PotentiometerImpl.h"        // potentiometer

#include <LiquidCrystal_I2C.h>  // LCD screen
#include <Wire.h>               // both needed for the

class HWPlatform {
public:
    HWPlatform();

    void init();  // inizializza tutto l'hardware (lcd, pins, ecc.)
    void test();  // inizializza i test

    /* ---------------------------------- ACCESSO AI DEVICE ---------------------------------- */

    // Motore porta hangar
    ServoMotor* getHangarDoorMotor();

    // LCD dell’operatore
    LiquidCrystal_I2C* getLcd();

    // Bottone di reset allarme
    ButtonImpl* getResetButton();

private:
    // istanze concrete dei device
    ServoMotor* pDoorMotor;
    LiquidCrystal_I2C* pLcd;
    ButtonImpl* pResetButton;
    PotentiometerImpl* pPotentiometer;
};

#endif
