#include "SonarTask.h"
#include <Arduino.h>
#include "config.h"
#include "kernel/Logger.h"

bool DEBUG = true;

SonarTask::SonarTask(Controller* pController) : pController(pController) {
    setWaterState(WaterState::Low);
}

void SonarTask::tick() {
    pController->sync();
    float level = pController->getDistance();
    Serial.print("DISTANZA ");
    Serial.println(level);

    pController->setWaterDistance(level);
}

void SonarTask::setWaterState(WaterState waterState) {
    this->state = state;
    stateTimestamp = millis();
    justEntered = true;
}

long SonarTask::elapsedTimeInState() {
    return millis() - stateTimestamp;
}