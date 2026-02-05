// #include <Arduino.h>
// #include "config.h"
// #include "kernel/WiFiConnection.h"
// #include "kernel/Scheduler.h"

// WiFiConnection* pWiFiConnection;

// unsigned long lastPublish = 0;
// const unsigned long PUBLISH_INTERVAL = 5000;  // ogni 5 secondi

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
    // // 
    // unsigned long now = millis();
    // if (now - lastPublish >= PUBLISH_INTERVAL) {
    //     lastPublish = now;

    //     if (pHWPlatform->getMQTTpublisher()->connected()) {
    //         const char* testTopic = FREQ_TOPIC;
    //         const char* testMsg = "Ciao dal test ESP32 - " __DATE__ " " __TIME__;

    //         Serial.print("Publish su ");
    //         Serial.print(testTopic);
    //         Serial.print(" → ");
    //         Serial.println(testMsg);

    //         pHWPlatform->getMQTTpublisher()->publish(testTopic, testMsg);  // usa il metodo semplice
    //         // Oppure publisher->publishJSON(...) se preferisci il tuo formato JSON
    //     } else {
    //         Serial.println("Publisher non connesso → skip publish");
    //     }
    // }

// }
