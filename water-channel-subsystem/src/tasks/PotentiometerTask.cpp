#include <Arduino.h>
#include "PotentiometerTask.h"

PotentiometerTask::PotentiometerTask(Controller* pController) : pController(pController) {
    pPotentiometer = pController->getHWPlatform()->getPotentiometer(); 
}

void PotentiometerTask::tick() {
    // Serial.print("PotentiometerTask::tick "); 
    potentiometerPosition = pPotentiometer->getPosition();
    // Serial.println(potentiometerPosition);
    pController->setPotentiometerPosition(potentiometerPosition);
}
