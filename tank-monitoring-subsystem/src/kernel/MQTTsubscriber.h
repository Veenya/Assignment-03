#ifndef __MQTT_SERVICE__
#define __MQTT_SERVICE__

#include <PubSubClient.h>
#include "MQTTservice.h"
#include <functional>               // ← AGGIUNGI QUESTA RIGA

class MQTTsubscriber : public MQTTservice {
public:
    MQTTsubscriber(const char* server, const char* client_id) 
        : MQTTservice(server, client_id) {}

    void subscribe(const char* topic);

    void callback(char* topic, byte* payload, unsigned int length);

    void subscribeJSON(const char* topic);
    
    void callbackJSON(char* topic, byte* payload, unsigned int length);
    
    void setSavedPayload(byte* payload);
    
    byte* getSavedPayload();
    
    void setSavedPayload(int payload);
    
    int getSavedPayloadInt();

    // Firma ESPLICITA invece del macro (funziona sempre su ESP32)
    void setCallback(std::function<void(char*, uint8_t*, unsigned int)> callback);

private:
    byte* saved_payload;
    int saved_payload_int;
};

#endif