#ifndef __BUTTON_TASK__
#define __BUTTON_TASK__

#include "model/Controller.h"
#include "model/HWPlatform.h"
// #include "model/States.h"
#include "config.h"
#include "devices/button/ButtonImpl.h"
#include "kernel/Task.h"

class ButtonTask : public Task {
public:
    explicit ButtonTask(Controller* pController);
    void tick();

private:
    HWPlatform* pHW;
    Controller* pController;
    ButtonImpl* pButton;

    float potentiometerPosition;
};

#endif
