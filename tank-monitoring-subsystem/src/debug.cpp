// #include <Arduino.h>
// #include "config.h"
// #include "kernel/WiFiConnection.h"
// #include "kernel/Scheduler.h"

// WiFiConnection* pWiFiConnection;

// void setup() {
//     if (true) {
//         Serial.begin(115200);
//         delay(200);
//         Serial.println("\n=== TEST WiFiConnection CLASS ===\n");

//         pWiFiConnection->setup_wifi();

//         if (pWiFiConnection->isConnected()) {
//             Serial.println("Tutto ok");
//         } else {
//             Serial.println("Connessione fallita → controlla credenziali e segnale");
//         }
//     }
// }

// void loop() {
//     if (true) {
//         static unsigned long lastCheck = 0;

//         if (millis() - lastCheck > 10000) {
//             lastCheck = millis();

//             if (pWiFiConnection->isConnected()) {
//                 Serial.print("[OK] Connesso - IP: ");
//                 Serial.println(pWiFiConnection->getLocalIP());
//             } else {
//                 Serial.println("[!!] Connessione persa - ritento...");
//                 pWiFiConnection->setup_wifi();
//             }
//         }
//     }
// }
