#include "CommunicationCenter.h"
#include <Arduino.h>
#include "kernel/MsgService.h"

CommunicationCenter::CommunicationCenter(Controller* sys) : pController(sys) {}

void CommunicationCenter::init() {
    newModeCmd = false;
    newValveCmd = false;

    // connectivity tracking
    lastRxMs = 0;
    // T2 timeout (ms) - set from config.h if you want
    T2_MS = 15000;
    Serial.println("Communication Center initialized");
}

void CommunicationCenter::notifyNewState() {
    // Send compact state snapshot to CUS
    String systemStateStr;
    String connectionStateStr;
    int valveOpening = pController->getValveOpening();
    
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
    // 1) Read incoming messages from CUS (serial)
    if (MsgService.isMsgAvailable()) {
        Msg* msg = MsgService.receiveMsg();
        if (msg != NULL) {
            String content = msg->getContent();
            // Any valid message means CUS is reachable -> CONNECTED
            lastRxMs = millis();
            pController->setConnectivity(ConnectivityState::CONNECTED);

            if (content == "PING") {
                // nothing else to do
            } else if (content.startsWith("MODE,")) {
                String m = content.substring(5);
                m.trim();
                if (m == "MANUAL_LOCAL") {
                    pController->setSystemState(SystemState::MANUAL_LOCAL);
                    newModeCmd = true;
                } else if (m == "MANUAL_REMOTE") {
                    pController->setSystemState(SystemState::MANUAL_REMOTE);
                    newModeCmd = true;
                } else if (m == "AUTO") {
                    pController->setSystemState(SystemState::AUTOMATIC);
                    newModeCmd = true;
                }
            } else if (content.startsWith("VALVE,")) {
                String vStr = content.substring(6);
                vStr.trim();
                int v = vStr.toInt();
                pController->setValveOpening(v);
                newValveCmd = true;
            } else if (content.startsWith("WL,")) {
                // optional: show WL sent by CUS on LCD
                String wlStr = content.substring(3);
                wlStr.trim();
                float waterLevel = wlStr.toFloat();
                pController->setWaterLevel(waterLevel);
            }

            delete msg;
        }
    }

    // 2) Connectivity timeout -> UNCONNECTED (spec-like)
    unsigned long now = millis();
    if (lastRxMs == 0) {
        // never received anything yet
        pController->setConnectivity(ConnectivityState::UNCONNECTED);
    } else if (now - lastRxMs > T2_MS) {
        pController->setConnectivity(ConnectivityState::UNCONNECTED);
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
