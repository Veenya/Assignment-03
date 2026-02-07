#ifndef __COMMUNICATIONCENTER__
#define __COMMUNICATIONCENTER__

#include "HWPlatform.h"
#include "config.h"
#include "model/Controller.h"

/*
 * Serial link between WCS (Arduino) and CUS (PC).
 *
 * Responsibilities:
 * - Read commands from CUS:
 *   - SET_MODE AUTO|MANUAL
 *   - SET_VALVE <0..100>
 *   - (optional) SET_WL <float>   // for LCD display
 *   - (optional) PING / HEARTBEAT
 *
 * - Send status to CUS:
 *   - STATUS <mode> <conn> <valve> <wl>
 *   - (optional) EVENT MODE_TOGGLED <mode>
 *
 * - Track connectivity: if no valid command for T2 -> UNCONNECTED.
 */

class CommunicationCenter {
public:
    explicit CommunicationCenter(Controller* sys);

    void init();

    // Send current state to CUS
    void notifyNewState();

    // Read messages from CUS + update UNCONNECTED based on timeout
    void sync();

    // Optional flags for tasks
    bool checkAndResetNewModeCmd();
    bool checkAndResetNewValveCmd();

private:
    Controller* pController;

    bool newModeCmd;
    bool newValveCmd;

    unsigned long lastRxMs;
    unsigned long T2_MS;
};

#endif