#include "CommunicationCenter.h"
#include <Arduino.h>
#include "kernel/MsgService.h"

CommunicationCenter::CommunicationCenter(Controller* sys) : pController(sys) {}

void CommunicationCenter::init() {
    lastSystemStateReceived = "MANUAL_LOCAL";
    newModeCmd = false;
    newValveCmd = false;

    lastRxMs = 0;
    T2_MS = 15000;
    Serial.println("Communication Center initialized");
}

void CommunicationCenter::notifyNewState() {
    // Send compact state snapshot to CUS
    String systemStateStr;
    String connectionStateStr;
    int valveOpening = pController->getValveOpening();
    // Serial.print("CommunicationCenter::notifyNewState ");
    // Serial.print(valveOpening);
    // Serial.print(" -> ");
    // Serial.println(String(valveOpening));
    
    if (pController->getSystemState() == SystemState::MANUAL_LOCAL) {
        systemStateStr = "MANUAL_LOCAL";
    } else if (pController->getSystemState() == SystemState::MANUAL_REMOTE) {
        systemStateStr = "MANUAL_REMOTE";
    } else if (pController->getSystemState() == SystemState::AUTOMATIC) {
        systemStateStr = "AUTOMATIC";
    }
    
    connectionStateStr = (pController->getConnectivity() == ConnectivityState::UNCONNECTED) ? "UNCONNECTED" : "CONNECTED";

    MsgService.sendMsg(
        systemStateStr + "," +
        connectionStateStr + "," +
        String(valveOpening)
    );
}

void CommunicationCenter::sync() {
    if (!MsgService.isMsgAvailable()) {
        checkConnectivityTimeout();
        return;
    }

    Msg* msg = MsgService.receiveMsg();
    if (msg == NULL) {
        checkConnectivityTimeout();
        return;
    }

    String content = msg->getContent();
    delete msg;

    lastRxMs = millis();
    pController->setConnectivityState(ConnectivityState::CONNECTED);

    Serial.print("Received from serial: ");
    Serial.println(content);

    // ---- PARSING A 4 CAMPI ----
    String fields[4];
    int fieldIndex = 0;

    int start = 0;
    while (fieldIndex < 4) {
        int comma = content.indexOf(',', start);
        if (comma == -1) {
            fields[fieldIndex++] = content.substring(start);
            break;
        }
        fields[fieldIndex++] = content.substring(start, comma);
        start = comma + 1;
    }

    // Trim di sicurezza
    for (int i = 0; i < fieldIndex; i++) {
        fields[i].trim();
    }

    // ---- DISPATCH ----
    // CUS,AUTO,5,10
    if (fields[0] == "CUS") {
        // MODE
        if (fields[1] != lastSystemStateReceived) {
            if (fields[1] == "MANUAL_LOCAL") {
                pController->setSystemState(SystemState::MANUAL_LOCAL);
            }
            else if (fields[1] == "MANUAL_REMOTE") {
                pController->setSystemState(SystemState::MANUAL_REMOTE);
            }
            else if (fields[1] == "AUTOMATIC" || fields[1] == "AUTO") {
                pController->setSystemState(SystemState::AUTOMATIC);
            }
            lastSystemStateReceived = fields[1];
            newModeCmd = true;
        }

        // VALVE
        int v = fields[2].toInt();
        pController->setValveOpening(v);
        newValveCmd = true;
        
        // WATER LEVEL
        int wl = fields[3].toInt();
        Serial.print("WATER LEVEL IS ");
        Serial.println(wl);
        pController->setWaterLevel(wl);

        Serial.print("CUS parsed -> ");
        Serial.print(fields[1]);
        Serial.print(", ");
        Serial.print(fields[2]);
        Serial.print(", ");
        Serial.println(fields[3]);

        // qui decidi tu cosa farne
    }

    checkConnectivityTimeout();
}

void CommunicationCenter::checkConnectivityTimeout() {
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
