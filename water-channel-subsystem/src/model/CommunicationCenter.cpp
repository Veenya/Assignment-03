#include "CommunicationCenter.h"
#include <Arduino.h>
#include <ArduinoJson.h>  // assicurati che sia incluso

CommunicationCenter::CommunicationCenter(Controller* sys) : pController(sys) {}

void CommunicationCenter::init() {
    lastSystemStateReceived = "MANUAL_LOCAL";
    newModeCmd = false;
    newValveCmd = false;

    lastRxMs = 0;
    T2_MS = 15000;  // timeout 15 secondi

    Serial.println("Communication Center initialized (JSON mode)");
}

void CommunicationCenter::notifyNewState() {
    stateDoc.clear();  // resetta il documento

    const char* stateStr;
    switch (pController->getSystemState()) {
        case SystemState::MANUAL_LOCAL:   stateStr = "MANUAL_LOCAL"; break;
        case SystemState::MANUAL_REMOTE:  stateStr = "MANUAL_REMOTE"; break;
        case SystemState::AUTOMATIC:      stateStr = "AUTOMATIC"; break;
        default:                          stateStr = "UNKNOWN"; break;
    }

    const char* connStr = (pController->getConnectivity() == ConnectivityState::CONNECTED)
                          ? "CONNECTED" : "UNCONNECTED";

    stateDoc["mode"]  = stateStr;
    stateDoc["conn"]  = connStr;
    stateDoc["valve"] = pController->getValveOpening();

    jsonBuffer = "";           // svuota
    serializeJson(stateDoc, jsonBuffer);

    MsgService.sendMsg(jsonBuffer);

    // Debug
    Serial.print("→ JSON sent: ");
    Serial.println(jsonBuffer);
}

void CommunicationCenter::syncStr() {
    if (MsgService.isMsgAvailable()) {
        Msg* msg = MsgService.receiveMsg();
        if (msg != NULL) {
            String content = msg->getContent();
            content.trim();

            // Qualsiasi messaggio valido → reset timeout → CONNECTED
            lastRxMs = millis();
            pController->setConnectivityState(ConnectivityState::CONNECTED);

            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, content);

            if (error) {
                Serial.print("JSON parse error: ");
                Serial.print(error.c_str());
                Serial.print(" → raw: ");
                Serial.println(content);
            } 
            else {
                // MODE command
                if (doc["cmd"].is<String>() && doc["cmd"] == "MODE") {
                    String modeValue = doc["value"] | "";
                    modeValue.trim();

                    if (modeValue.length() == 0) {
                        // valore mancante → skip
                    }
                    else if (modeValue == lastSystemStateReceived) {
                        // già nello stato richiesto → skip
                    }
                    else if (modeValue == "MANUAL_LOCAL") {
                        pController->setSystemState(SystemState::MANUAL_LOCAL);
                        lastSystemStateReceived = "MANUAL_LOCAL";
                        newModeCmd = true;
                    }
                    else if (modeValue == "MANUAL_REMOTE") {
                        pController->setSystemState(SystemState::MANUAL_REMOTE);
                        lastSystemStateReceived = "MANUAL_REMOTE";
                        newModeCmd = true;
                    }
                    else if (modeValue == "AUTO" || modeValue == "AUTOMATIC") {
                        pController->setSystemState(SystemState::AUTOMATIC);
                        lastSystemStateReceived = "AUTOMATIC";
                        newModeCmd = true;
                    }
                    else {
                        Serial.print("Modalità sconosciuta ricevuta: ");
                        Serial.println(modeValue);
                    }
                }

                // VALVE command
                if (doc["cmd"].is<String>() && doc["cmd"] == "VALVE") {
                    int v = doc["value"] | -1;
                    if (v >= 0 && v <= 100) {
                        pController->setValveOpening(v);
                        newValveCmd = true;
                    } else {
                        Serial.print("Valore valvola fuori range (0-100): ");
                        Serial.println(v);
                    }
                }

                // Water Level (opzionale)
                if (doc["wl"].is<float>()) {
                    float waterLevel = doc["wl"];
                    if (waterLevel >= 0.0f && waterLevel <= 200.0f) {  // range realistico per un serbatoio
                        pController->setWaterLevel(waterLevel);
                    } else {
                        Serial.print("Livello acqua fuori range: ");
                        Serial.println(waterLevel);
                    }
                }
            }

            delete msg;
        }
    }

    // Timeout → UNCONNECTED
    unsigned long now = millis();
    if (lastRxMs == 0 || now - lastRxMs > T2_MS) {
        pController->setConnectivityState(ConnectivityState::UNCONNECTED);
    }
}

// Metodi legacy con protocollo stringa (per debug o fallback)
void CommunicationCenter::notifyNewStateStr() {
    String systemStateStr;
    String connectionStateStr;
    int valveOpening = pController->getValveOpening();

    Serial.print("notifyNewStateStr - valve: ");
    Serial.println(valveOpening);

    switch (pController->getSystemState()) {
        case SystemState::MANUAL_LOCAL:   systemStateStr = "MANUAL_LOCAL";   break;
        case SystemState::MANUAL_REMOTE:  systemStateStr = "MANUAL_REMOTE";  break;
        case SystemState::AUTOMATIC:      systemStateStr = "AUTOMATIC";      break;
        default:                          systemStateStr = "UNKNOWN";        break;
    }

    connectionStateStr = (pController->getConnectivity() == ConnectivityState::UNCONNECTED)
                         ? "UNCONNECTED" : "CONNECTED";

    String payload = systemStateStr + "," + connectionStateStr + "," + String(valveOpening);
    MsgService.sendMsg(payload);
}

void CommunicationCenter::sync() {
    if (MsgService.isMsgAvailable()) {
        Msg* msg = MsgService.receiveMsg();
        if (msg != NULL) {
            String content = msg->getContent();
            lastRxMs = millis();
            pController->setConnectivityState(ConnectivityState::CONNECTED);

            if (content == "PING") {
                // nop
            } else if (content.startsWith("MODE,")) {
                String m = content.substring(5);  // "MODE," = 5 char
                m.trim();
                if (m == lastSystemStateReceived) {
                    // skip
                } else if (m == "MANUAL_LOCAL") {
                    pController->setSystemState(SystemState::MANUAL_LOCAL);
                    lastSystemStateReceived = "MANUAL_LOCAL";
                    newModeCmd = true;
                } else if (m == "MANUAL_REMOTE") {
                    pController->setSystemState(SystemState::MANUAL_REMOTE);
                    lastSystemStateReceived = "MANUAL_REMOTE";
                    newModeCmd = true;
                } else if (m == "AUTO") {
                    pController->setSystemState(SystemState::AUTOMATIC);
                    lastSystemStateReceived = "AUTO";
                    newModeCmd = true;
                }
            } else if (content.startsWith("VALVE,")) {
                String vStr = content.substring(6);
                vStr.trim();
                int v = vStr.toInt();
                if (v >= 0 && v <= 100) {
                    pController->setValveOpening(v);
                    newValveCmd = true;
                }
            } else if (content.startsWith("WL,")) {
                String wlStr = content.substring(3);
                wlStr.trim();
                float waterLevel = wlStr.toFloat();
                pController->setWaterLevel(waterLevel);
            }

            delete msg;
        }
    }

    unsigned long now = millis();
    if (lastRxMs == 0 || now - lastRxMs > T2_MS) {
        pController->setConnectivityState(ConnectivityState::UNCONNECTED);
    }
}

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