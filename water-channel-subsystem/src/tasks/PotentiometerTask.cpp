#include <Arduino.h>
#include "PotentiometerTask.h"

// Set false to disable debug prints
static bool DEBUG = true;

PotentiometerTask::PotentiometerTask(Controller* pController) : pController(pController) {
    pHW = pController->getHWPlatform();
    pPotentiometer = pHW->getPotentiometer(); 
}

void PotentiometerTask::tick() {
    potentiometerPosition = pPotentiometer->getPosition();
    pController->setPotentiometerPosition(potentiometerPosition);
}
