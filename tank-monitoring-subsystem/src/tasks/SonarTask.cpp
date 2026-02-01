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

    if (!DEBUG) {
        switch (state) {
            case WaterState::Low: {
                pController->setWaterState(WaterState::Low);

                if (level > L1) {
                    setWaterState(WaterState::Medium);
                }
                break;
            }

            case WaterState::Medium: {
                if (level < L1) {
                    setWaterState(WaterState::Low);
                } else if (level > L1 && elapsedTimeInState() > TIME1) {
                    setWaterState(WaterState::High);
                }
                break;
            }

            case WaterState::High: {
                // ? Può tornare low?
                if (level < L1) {
                    setWaterState(WaterState::Low);
                }
                break;
            }
        }  // end switch
        pController->setWaterState(this->state);
    }
}

void SonarTask::setWaterState(WaterState waterState) {
    this->state = state;
    stateTimestamp = millis();
    justEntered = true;
}

long SonarTask::elapsedTimeInState() {
    return millis() - stateTimestamp;
}