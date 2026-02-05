#ifndef WIFI_CONNECTION
#define WIFI_CONNECTION

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include "config.h"

class WiFiConnection {
public:
    WiFiConnection(const char* ssid, const char* password);

    void setup_wifi();
    wl_status_t status() const;
    bool isConnected() const;

    // Metodi di utility (opzionali ma utili per debug)
    void printStatus() const;
    IPAddress getLocalIP() const;
    String getSSID() const;
    int getRSSI() const;

private:
    const char* _ssid;
    const char* _password;

    static const unsigned long CONNECT_TIMEOUT_MS = 15000;  // 15 secondi max
};

#endif