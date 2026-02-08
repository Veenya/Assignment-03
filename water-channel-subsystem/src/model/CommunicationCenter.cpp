#include "CommunicationCenter.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include "kernel/MsgService.h"

CommunicationCenter::CommunicationCenter(Controller* sys) : pController(sys) {}

void CommunicationCenter::init() {
    lastSystemStateReceived = "MANUAL_LOCAL";
    newModeCmd = false;
    newValveCmd = false;

    // connectivity tracking
    lastRxMs = 0;
    // T2 timeout (ms) - set from config.h if you want
    T2_MS = 15000;
    Serial.println("Communication Center initialized");
}

// void CommunicationCenter::notifyNewState() {
//     // Send compact state snapshot to CUS
//     String systemStateStr;
//     String connectionStateStr;
//     int valveOpening = pController->getValveOpening();
//     Serial.print("CommunicationCenter::notifyNewState ");
//     Serial.print(valveOpening);
//     Serial.print(" -> ");
//     Serial.println(String(valveOpening));
    
//     if (pController->getSystemState() == SystemState::MANUAL_LOCAL) {
//         systemStateStr = "MANUAL_LOCAL";
//     } else if (pController->getSystemState() == SystemState::MANUAL_REMOTE) {
//         systemStateStr = "MANUAL_REMOTE";
//     } else if (pController->getSystemState() == SystemState::AUTOMATIC) {
//         systemStateStr = "AUTOMATIC";
//     }
    
//     connectionStateStr = (pController->getConnectivity() == ConnectivityState::UNCONNECTED) ? "UNCONNECTED" : "CONNECTED";

//     MsgService.sendMsg(
//         systemStateStr + "," +
//         connectionStateStr + "," +
//         String(valveOpening)
//     );
// }


void CommunicationCenter::notifyNewState() {
  StaticJsonDocument<128> doc; // Contenitore JSON con dimensione massima di 128 byte

  // Legge lo stato del sistema e lo converte in stringa
  const char* mode =
    (pController->getSystemState() == SystemState::MANUAL_LOCAL)  ? "MANUAL_LOCAL" :
    (pController->getSystemState() == SystemState::MANUAL_REMOTE) ? "MANUAL_REMOTE" :
                                                                    "AUTOMATIC";

  // Legge la connettivita' del controller e la converte in stringa                                                                  
//   const char* conn =
//     (pController->getConnectivity() == ConnectivityState::UNCONNECTED) ? "UNCONNECTED" : "CONNECTED";

    doc["valveValue"] = pController->getValveOpening();  
    doc["mode"] = mode;
  

    String out;
    serializeJson(doc, out);
    MsgService.sendMsg(out);             
}


// void CommunicationCenter::sync() {
//     // 1) Read incoming messages from CUS (serial)
//     if (MsgService.isMsgAvailable()) {
//         Msg* msg = MsgService.receiveMsg();
//         if (msg != NULL) {
//             String content = msg->getContent();
//             // Any valid message means CUS is reachable -> CONNECTED
//             lastRxMs = millis();
//             pController->setConnectivityState(ConnectivityState::CONNECTED);

//             if (content == "PING") {
//                 // nothing else to do
//             } else if (content.startsWith("MODE,")) {
//                 String m = content.substring(13); // TODO rivedere
//                 m.trim();
//                 if (m == lastSystemStateReceived) {
//                     //! SKIP, non fare nulla
//                 } else if (m == "MANUAL_LOCAL") {
//                     pController->setSystemState(SystemState::MANUAL_LOCAL);
//                     lastSystemStateReceived = "MANUAL_LOCAL";
//                     newModeCmd = true;
//                 } else if (m == "MANUAL_REMOTE") {
//                     pController->setSystemState(SystemState::MANUAL_REMOTE);
//                     lastSystemStateReceived = "MANUAL_REMOTE";
//                     newModeCmd = true;
//                 } else if (m == "AUTO") {
//                     pController->setSystemState(SystemState::AUTOMATIC);
//                     lastSystemStateReceived = "AUTO";
//                     newModeCmd = true;
//                 }
//             } else if (content.startsWith("VALVE,")) {
//                 String vStr = content.substring(6);
//                 vStr.trim();
//                 int v = vStr.toInt();
//                 pController->setValveOpening(v);
//                 newValveCmd = true;
//             } else if (content.startsWith("WL,")) {
//                 // optional: show WL sent by CUS on LCD
//                 String wlStr = content.substring(3);
//                 wlStr.trim();
//                 float waterLevel = wlStr.toFloat();
//                 pController->setWaterLevel(waterLevel);
//             }

//             delete msg;
//         }
//     }

//     // 2) Connectivity timeout -> UNCONNECTED (spec-like)
//     unsigned long now = millis();
//     if (lastRxMs == 0 || now - lastRxMs > T2_MS) {
//         // never received anything yet
//         pController->setConnectivityState(ConnectivityState::UNCONNECTED);
//     }
// }


void CommunicationCenter::sync() {
  if (MsgService.isMsgAvailable()) {
    Msg* msg = MsgService.receiveMsg();
    if (msg != NULL) {
      String content = msg->getContent();
      lastRxMs = millis();
      pController->setConnectivityState(ConnectivityState::CONNECTED);

      StaticJsonDocument<192> doc;
      DeserializationError err = deserializeJson(doc, content);

      if (!err) {
        //const char* type = doc["type"] | "";

        // --- comando mode ---
        if (strcmp(type, "cmd") == 0 && doc.containsKey("isManual")) {
          bool isManual = doc["isManual"];
          if (isManual) {
            pController->setSystemState(SystemState::MANUAL_REMOTE);  // o MANUAL_LOCAL se vuoi
            lastSystemStateReceived = "MANUAL_REMOTE";
          } else {
            pController->setSystemState(SystemState::AUTOMATIC);
            lastSystemStateReceived = "AUTOMATIC";
          }
          newModeCmd = true;
        }

        // --- comando valve ---
        if (strcmp(type, "cmd") == 0 && doc.containsKey("valveValue")) {
          int v = doc["valveValue"];
          pController->setValveOpening(v);
          newValveCmd = true;
        }

        // --- opzionale: wl da CUS ---
        // if (strcmp(type, "wl") == 0 && doc.containsKey("wl")) {
        //   float wl = doc["wl"];
        //   pController->setWaterLevel(wl);
        // }

      }
      delete msg;
    }
  }

  unsigned long now = millis();
  if (lastRxMs == 0 || now - lastRxMs > T2_MS) {
    pController->setConnectivityState(ConnectivityState::UNCONNECTED);
  }
}

/* -------- Optional "checkAndReset" for tasks -------- */

bool CommunicationCenter::checkAndResetNewModeCmd() {
    bool r = newModeCmd;
    newModeCmd = false;
    return r;
}

bool CommunicationCenter::checkAndResetNewValveCmd() {
    bool r = newValveCmd;
    newValveCmd = false;
    return r;
}
