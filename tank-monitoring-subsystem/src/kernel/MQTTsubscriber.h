#ifndef __MQTT_SUBSCRIBER__
#define __MQTT_SUBSCRIBER__

#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "kernel/MQTTservice.h"
#include "config.h"

class MQTTsubscriber : public MQTTservice {
public:
    MQTTsubscriber(const char* server, const char* client_id);

    void begin();                               // sostituisce subscribeJSON
    void loop();
    boolean isConnected();
    int getSavedFrequency() const;

private:
    int saved_frequency = 0;

    // Callback interno
    void handleMessage(char* topic, byte* payload, unsigned int length);
    void processFrequencyPayload(byte* payload, unsigned int length);
};

#endif