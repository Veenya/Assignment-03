#include <Arduino.h>
#include "model/Controller.h"
#include "ControllerTask.h"


ControllerTask::ControllerTask(Controller* pController, CommunicationCenter* pCommunicationCenter, UserPanel* pUserPanel)
    : 
    pController(pController),
    pCommunicationCenter(pCommunicationCenter),
    pUserPanel(pUserPanel),
    systemState(SystemState::MANUAL_LOCAL),
    connectivityState(ConnectivityState::UNCONNECTED),
    waterLevel(0.0f),
    valveOpening(0)
    {}


void ControllerTask::tick() {
    Serial.println("Controller task tick");
    this->connectivityState = pController->getConnectivityState();
    checkSystemState();
    manageValve();
    updateDisplay();
}


/* --------- Operator inputs --------- */
// bool ControllerTask::isModeButtonPressed() {
//     Serial.println("isModeButtonPressed inizio");
//     delay(1000);
//     return pController->buttonCheckAndConsumeClick();
// }

int ControllerTask::readManualValveFromPot() {
    pPotentiometer->sync();

    // Your PotentiometerImpl::position() should already be 0..100
    int percent = (int)pPotentiometer->getPosition();
    return pController->clampPercent(percent);
}

void ControllerTask::updateDisplay() {
    if (connectivityState == ConnectivityState::UNCONNECTED) {
        pUserPanel->displayUnconnected();
    } else if (systemState == SystemState::MANUAL_LOCAL) {
        pUserPanel->displayManualLocal();
    } else if (systemState == SystemState::MANUAL_REMOTE) {
        pUserPanel->displayManualRemote();
    } else if (systemState == SystemState::AUTOMATIC) {
        pUserPanel->displayAutomatic();
    }

    pUserPanel->displayWaterLevel(waterLevel);
}

void ControllerTask::refreshOutputs() {
    applyValveToServo();
    updateDisplay();
}

void ControllerTask::checkSystemState() {
    // entrambi sono check and reset del flag
    Serial.println("checkSystemState");
    if (pController->buttonCheckAndConsumeClick()) {
        Serial.println("check if pressed");
        pController->setSystemState(SystemState::MANUAL_LOCAL);
        this->systemState = SystemState::MANUAL_LOCAL;
    }
    if (pCommunicationCenter->checkAndResetNewModeCmd()) {
        Serial.println("check if new cmd");
        this->systemState = pController->getSystemState();
    }
}

void ControllerTask::manageValve() {
    Serial.print("ControllerTask::manageValve ");
    Serial.print(pController->getPotentiometerPosition());
    Serial.print(" -> ");
    if (systemState == SystemState::MANUAL_LOCAL) {
        int valvePercent = pController->getPotentiometerPosition();
        setValveOpening(valvePercent);
        pController->setValveOpening(valvePercent);
    } else if (systemState != SystemState::MANUAL_LOCAL && connectivityState == ConnectivityState::UNCONNECTED) { // se disconnesso chiudo
        setValveOpening(0);
    } else if (connectivityState == ConnectivityState::CONNECTED) {
        setValveOpening(pController->getValveOpening());
    }
    Serial.println(pController->getValveOpening());
    applyValveToServo();
}

void ControllerTask::applyValveToServo() {
    // Safety: if UNCONNECTED, force closed
    Serial.print("ControllerTask::applyValveToServo ");
    Serial.print(valveOpening);
    int effectivePercent;
    if (systemState != SystemState::MANUAL_LOCAL && connectivityState == ConnectivityState::UNCONNECTED) {
        effectivePercent = 0;
    } else {
        effectivePercent = valveOpening;
    }
    Serial.print(" -> ");
    int angle = pController->percentToServoAngle(effectivePercent);
    Serial.println(angle);
    pController->moveMotor(angle);
}



void ControllerTask::setSystemState(SystemState systemMode) {
    systemState = systemMode;
}

SystemState ControllerTask::getSystemState() const {
    return systemState;
}

void ControllerTask::setConnectivityState(ConnectivityState connectivityState) {
    connectivityState = connectivityState;
}

ConnectivityState ControllerTask::getConnectivity() const {
    return connectivityState;
}

bool ControllerTask::isManual() const {
    return systemState == SystemState::MANUAL_LOCAL;
}

bool ControllerTask::isUnconnected() const {
    return connectivityState == ConnectivityState::UNCONNECTED;
}

/* --------- Water level (optional for LCD) --------- */

void ControllerTask::setWaterLevel(float waterLevel) {
    waterLevel = waterLevel;
}

float ControllerTask::getWaterLevel() const {
    return waterLevel;
}

/* --------- Valve control --------- */

void ControllerTask::setValveOpening(int percent) {
    valveOpening = percent;
}

int ControllerTask::getValveOpening() const {
    return valveOpening;
}