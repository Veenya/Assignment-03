#include "CommunicationCenter.h"
#include <Arduino.h>
#include "kernel/MsgService.h"

// Commands from CUS -> WCS (examples):
//   "MODE,AUTO"
//   "MODE,MANUAL"
//   "VALVE,50"
//   "WL,12.3"          (optional; for LCD)
//   "PING"             (optional; heartbeat)
//
// Status from WCS -> CUS:
//   "STATE,<MODE>,<CONN>,<VALVE>,<WL>"
//   example: "STATE,AUTO,CONNECTED,50,12.3"

CommunicationCenter::CommunicationCenter(Controller* sys) : pSys(sys) {}

void CommunicationCenter::init() {
    // flags for tasks (optional)
    newModeCmd = false;
    newValveCmd = false;

    // connectivity tracking
    lastRxMs = 0;
    // T2 timeout (ms) - set from config.h if you want
    T2_MS = 15000;
}

void CommunicationCenter::notifyNewState() {
    // Send compact state snapshot to CUS
    String modeStr = (pSys->getMode() == SystemState::MANUAL) ? "MANUAL" : "AUTO";
    String connStr = (pSys->getConnectivity() == ConnectivityState::UNCONNECTED) ? "UNCONNECTED" : "CONNECTED";

    int valve = pSys->getValveOpening();
    float wl = pSys->getWaterLevel(); // if you don't use WL on WCS, send 0.0

    MsgService.sendMsg(
        String("STATE,") +
        modeStr + "," +
        connStr + "," +
        String(valve) + "," +
        String(wl, 1)
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
            pSys->setConnectivity(ConnectivityState::CONNECTED);

            // Parse CSV-like messages
            // Expected formats:
            // MODE,AUTO
            // MODE,MANUAL
            // VALVE,NN
            // WL,XX.X
            // PING
            if (content == "PING") {
                // nothing else to do
            } else if (content.startsWith("MODE,")) {
                String m = content.substring(5);
                m.trim();
                if (m == "MANUAL") {
                    pSys->setMode(SystemState::MANUAL);
                    newModeCmd = true;
                } else if (m == "AUTO") {
                    pSys->setMode(SystemState::AUTOMATIC);
                    newModeCmd = true;
                }
            } else if (content.startsWith("VALVE,")) {
                String vStr = content.substring(6);
                vStr.trim();
                int v = vStr.toInt();
                pSys->setValveOpening(v);
                newValveCmd = true;
            } else if (content.startsWith("WL,")) {
                // optional: show WL sent by CUS on LCD
                String wlStr = content.substring(3);
                wlStr.trim();
                float wl = wlStr.toFloat();
                pSys->setWaterLevel(wl);
            }

            delete msg;
        }
    }

    // 2) Connectivity timeout -> UNCONNECTED (spec-like)
    unsigned long now = millis();
    if (lastRxMs == 0) {
        // never received anything yet
        pSys->setConnectivity(ConnectivityState::UNCONNECTED);
    } else if (now - lastRxMs > T2_MS) {
        pSys->setConnectivity(ConnectivityState::UNCONNECTED);
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
