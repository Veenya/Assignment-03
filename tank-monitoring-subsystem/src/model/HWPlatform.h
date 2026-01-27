/* Contiene tutti gli oggetti device e usa i pin di config.h. */
#ifndef __HW_PLATFORM__
#define __HW_PLATFORM__

#include "config.h"  // pins

#include "devices/led/Led.h"                        // led
#include "devices/proximity_sensor/Sonar.h"         // proximity sensor

class HWPlatform {
public:
    HWPlatform();

    void init();  // inizializza tutto l'hardware (pins, ecc.)

    /* ---------------------------------- ACCESSO AI DEVICE ---------------------------------- */
    Sonar* getDDD();

    // LED
    Led* getL1();  // verde network OK
    Led* getL2();  // rosso network KO


private:
    // istanze concrete dei device
    Sonar* pDdd;
    Led* pL1;
    Led* pL2;
};

#endif
