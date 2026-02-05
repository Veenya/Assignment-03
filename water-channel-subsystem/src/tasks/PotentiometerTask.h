#ifndef __POTENTIOMETER_TASK__
#define __POTENTIOMETER_TASK__

#include "model/HWPlatform.h"
#include "model/Controller.h"
#include "model/States.h"
#include "kernel/Task.h"
#include "config.h"
#include "devices/potentiometer/PotentiometerImpl.h"

/*
 * WCS-side system state and I/O:
 * - mode: AUTOMATIC / MANUAL
 * - connectivity: CONNECTED / UNCONNECTED
 * - valve opening: 0..100%
 * - operator inputs: button toggle, potentiometer value
 * - outputs: servo position, LCD text
 */
class PotentiometerTask : public Task {
public:
    explicit PotentiometerTask(Controller* pController);
    void tick();


    void getPotentiometerLevel();

private:
    HWPlatform* pHW;
    Controller* pController;
    PotentiometerImpl* pPotentiometer;

    float potentiometerPosition;

};

#endif
