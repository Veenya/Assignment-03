#ifndef __COMMUNICATIONCENTER__
#define __COMMUNICATIONCENTER__

#include "HWPlatform.h"
#include "config.h"
#include "model/Controller.h"

/*
 * Classe che comunica con il con il DRU (Drone Remote Unit) sul PC.
 * - Riceve comandi testuali via seriale (TAKEOFF, LANDING, RESET).
 * - Espone metodi "checkAndReset..." per i Task.
 * - Invia periodicamente lo stato corrente al DRU.
 */
class CommunicationCenter {
public:
    CommunicationCenter(Controller* pController);
    void init();
    void sync();
    void notifyNewState();

    bool checkAndResetOpenDoorRequest();  // TODO eliminare



private:
    Controller* pController;
    float waterDistance;
    float currentDistance;
    WaterState waterState;
    MQTTState mqttState;
};

#endif
