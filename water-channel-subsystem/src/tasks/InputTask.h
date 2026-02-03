#ifndef __INPUT_TASK__
#define __INPUT_TASK__

#include "kernel/Task.h"
#include "model/TankSystem.h"
#include "devices/button/ButtonImpl.h"
#include "devices/potentiometer/PotentiometerImpl.h"
//TODO: serve il servo?
class InputTask : public Task {
public:
    InputTask(TankSystem* pTankSystem, UserPanel* pUserPanel, Button* button, PotentiometerImpl* pot);
    void tick();

private:
    void handleButtonPress();
    void toggleMode();
    void handlePotentiometer();
    long elapsedTimeInState();
    void handleModeChange();
    void handleManualMode();
    void handleAutomaticMode();

    long stateTimestamp;
    bool justEntered;

    bool pressed;
    unsigned long lastButtonPress;

    SystemMode state;

    Button* button;
    PotentiometerImpl* pot;
    UserPanel* pUserPanel;
    TankSystem* pTankSystem;
    
    bool lastButtonState;
    unsigned long lastDebounceTime;
    static const unsigned long DEBOUNCE_DELAY = 50;

    int lastPotValue;
};

#endif