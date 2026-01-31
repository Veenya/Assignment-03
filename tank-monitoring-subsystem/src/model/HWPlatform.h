/* Contiene tutti gli oggetti device e usa i pin di config.h. */
#ifndef __HW_PLATFORM__
#define __HW_PLATFORM__

#include "config.h"  // pins
#include "model/States.h"

#include "devices/led/Led.h"                        // led
#include "devices/proximity_sensor/Sonar.h"         // proximity sensor

#include "kernel/MQTTpublisher.h"
#include "kernel/MQTTsubscriber.h"
#include "kernel/WiFiConnection.h"

class HWPlatform {
public:
    HWPlatform();

    void init();  // inizializza tutto l'hardware (pins, ecc.)

    /* ---------------------------------- ACCESSO AI DEVICE ---------------------------------- */
    Sonar* getDDD();

    // LED
    Led* getL1();  // verde network OK
    Led* getL2();  // rosso network KO

    WiFiConnection* getWiFiConnection();
    MQTTsubscriber* getMQTTsubscriber();
    MQTTpublisher* getMQTTpublisher();
    MQTTState getMQTTState();
    


private:
    // istanze concrete dei device
    Sonar* pDdd;
    Led* pL1;
    Led* pL2;
    MQTTState mqttState;
    WiFiConnection* pWiFiConnection;
    MQTTsubscriber* pMQTTsubscriber;
    MQTTpublisher* pMQTTpublisher;
};

#endif
