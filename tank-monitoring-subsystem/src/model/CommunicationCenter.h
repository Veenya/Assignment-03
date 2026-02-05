#ifndef __COMMUNICATIONCENTER__
#define __COMMUNICATIONCENTER__

#include "HWPlatform.h"
#include "config.h"
#include "model/Controller.h"
#include "kernel/MQTTpublisher.h"
#include "kernel/MQTTsubscriber.h"
#include "kernel/WiFiConnection.h"

class CommunicationCenter {
public:
    CommunicationCenter(Controller* pController);
    void init();
    void notifyNewState();
    bool checkMQTTConnection();

    WiFiConnection* getWiFiConnection();
    MQTTsubscriber* getMQTTsubscriber();
    MQTTpublisher* getMQTTpublisher();
    MQTTState getMQTTState();

private:
    Controller* pController;
    float waterDistance;
    float currentDistance;
    WaterState waterState;
    MQTTState mqttState;
    WiFiConnection* pWiFiConnection;
    MQTTsubscriber* pMQTTsubscriber;
    MQTTpublisher* pMQTTpublisher;
};

#endif
