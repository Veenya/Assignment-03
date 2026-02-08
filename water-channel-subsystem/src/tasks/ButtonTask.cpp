#include <Arduino.h>
#include "ButtonTask.h"

ButtonTask::ButtonTask(Controller* pController) : pController(pController) {
    pHW = pController->getHWPlatform();
    pButton = pHW->getButton(); 
}

void ButtonTask::tick() {
    // Serial.print("ButtonTask::tick "); 
    pButton->sync();
    // pController->syncButton();
}
