/* Contiene tutti gli oggetti device e usa i pin di config.h. */
#ifndef __HW_PLATFORM__
#define __HW_PLATFORM__

#include "config.h"  // pins

#include "devices/button/ButtonImpl.h"              // button
#include "devices/servo_motor/servo_motor_impl.h"        // servo motor
#include "devices/potentiometer/PotentiometerImpl.h"        // potentiometer

#include <LiquidCrystal_I2C.h>  // LCD screen
#include <Wire.h>               // both needed for the

class HWPlatform {
public:
    HWPlatform();

    void init();  // inizializza tutto l'hardware (lcd, pins, ecc.)

    // Motore della valvola
    ServoMotorImpl* getMotor();

    // LCD dell’operatore
    LiquidCrystal_I2C* getLcd();

    // Bottone di toggle modalita' auto/manual
    ButtonImpl* getToggleButton();

    // Potenziometro che serve a controllare il motore se in manual mode
    PotentiometerImpl* getPotentiometer();
    HWPlatform* getHWPlatform();

private:
    // istanze concrete dei device
    ServoMotorImpl* pMotor;
    LiquidCrystal_I2C* pLcd;
    ButtonImpl* pButton;
    PotentiometerImpl* pPotentiometer;
};

#endif
