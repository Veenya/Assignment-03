#include <Arduino.h>
#include "PotentiometerTask.h"

PotentiometerTask::PotentiometerTask(Controller* pController) : pController(pController) {
    pHW = pController->getHWPlatform();
    pPotentiometer = pHW->getPotentiometer(); 
}

void PotentiometerTask::tick() {
    potentiometerPosition = pPotentiometer->getPosition();
    pController->setPotentiometerPosition(potentiometerPosition);
}
