#ifndef __INPUT_TASK__
#define __INPUT_TASK__

#include "system/Task.h"
#include "WaterChannelController.h"
#include "components/api/Button.h"
#include "components/api/Potentiometer.h"

/*
 * InputTask (WCS)
 * - Handles local human interaction
 * - Button: toggle MANUAL / AUTOMATIC
 * - Potentiometer: valve value in MANUAL mode only
 *
 * Runs every ~50 ms
 */
class InputTask : public Task {
public:
    InputTask(
        int period,
        WaterChannelController* controller,
        int buttonPin,
        int potPin
    );

    void tick() override;

private:
    void handleButton();
    void handlePotentiometer();

    WaterChannelController* controller;

    Button* button;
    Potentiometer* potentiometer;

    bool lastButtonState;
    unsigned long lastDebounceTime;
    static const unsigned long DEBOUNCE_DELAY = 50;

    int lastPotValue;
};

#endif
