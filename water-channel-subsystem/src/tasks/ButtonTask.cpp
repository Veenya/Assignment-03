#include <Arduino.h>
#include "ButtonTask.h"

ButtonTask::ButtonTask(Controller* pController) : pController(pController) {
    pButton = pController->getHWPlatform()->getButton(); 
}

void ButtonTask::tick() {
    // Serial.print("ButtonTask::tick "); 
    pButton->sync();
    // pController->syncButton();
}
