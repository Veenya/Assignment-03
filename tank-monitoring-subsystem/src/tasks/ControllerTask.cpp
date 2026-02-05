#include "ControllerTask.h"
#include <Arduino.h>
#include "config.h"
// #include "kernel/Logger.h"

ControllerTask::ControllerTask(Controller* pController, CommunicationCenter* pCommunicationCenter)
    : pController(pController), pCommunicationCenter(pCommunicationCenter) {
    pHW = pController->getHWPlatform();
}

void ControllerTask::tick() {
    this->waterState = pController->getWaterState();
    this->mqttState = pController->getMQTTState();
    manageLeds();
    manageWaterLevel();
}

void ControllerTask::manageLeds() {
    if (pCommunicationCenter->getMQTTState() == MQTTState::CONNECTED) {
        pHW->getL1()->switchOn();
        pHW->getL2()->switchOff();
    } else {
        pHW->getL2()->switchOn();
        pHW->getL1()->switchOff();
    }
}

void ControllerTask::manageWaterLevel() {
    float level = pController->getDistance();
    switch (pController->getWaterState()) {
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
    pController->setWaterState(this->waterState);

}

void ControllerTask::setWaterState(WaterState waterState) {
    this->waterState = waterState;
    stateTimestamp = millis();
    justEntered = true;
}


long ControllerTask::elapsedTimeInState() {
    return millis() - stateTimestamp;
}