#include "WiFiConnection.h"

WiFiConnection::WiFiConnection(const char* ssid, const char* password)
    : _ssid(ssid), _password(password) {}

void WiFiConnection::setup_wifi() {
    Serial.println("\n=== Inizio connessione WiFi ===");
    Serial.print("SSID: ");
    Serial.println(_ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid, _password);

    Serial.print("Connessione in corso ");

    unsigned long startAttemptTime = millis();

    // Attende la connessione o timeout
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < CONNECT_TIMEOUT_MS) {
        delay(400);
        Serial.print(".");
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("→ Connesso con successo!");
        printStatus();
    } else {
        Serial.println("→ ERRORE: connessione fallita");
        Serial.println("Possibili cause:");
        Serial.println("  - SSID o password errati");
        Serial.println("  - Rete 5 GHz non supportata");
        Serial.println("  - Segnale troppo debole");
        Serial.println("  - Timeout raggiunto");
    }
}

wl_status_t WiFiConnection::status() const {
    return WiFi.status();
}

bool WiFiConnection::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

void WiFiConnection::printStatus() const {
    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("   Indirizzo IP : ");
        Serial.println(WiFi.localIP());
        Serial.print("   RSSI         : ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
        Serial.print("   MAC address  : ");
        Serial.println(WiFi.macAddress());
    } else {
        Serial.print("Stato WiFi: ");
        Serial.println(WiFi.status());
    }
}

IPAddress WiFiConnection::getLocalIP() const {
    return WiFi.localIP();
}

String WiFiConnection::getSSID() const {
    return WiFi.SSID();
}

int WiFiConnection::getRSSI() const {
    return WiFi.RSSI();
}