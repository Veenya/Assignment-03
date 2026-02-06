#include <Arduino.h>
#include "model/Controller.h"
#include "ControllerTask.h"

ControllerTask::ControllerTask(Controller* pController, CommunicationCenter* pCommunicationCenter, UserPanel* pUserPanel)
    : 
    pController(pController),
    pCommunicationCenter(pCommunicationCenter),
    pUserPanel(pUserPanel),
    systemState(SystemState::AUTOMATIC),
    connectivityState(ConnectivityState::UNCONNECTED),
    waterLevel(0.0f),
    valveOpening(0),
    lastButtonState(false)
    {
        pServo = pHW->getMotor();
        pPotentiometer = pHW->getPotentiometer();
        pBtn = pHW->getButton();
    }


void ControllerTask::tick() {
    Serial.println("Controller task tick");
    this->connectivityState = pController->getConnectivityState();
    // checkSystemState();
    manageValve();
    updateDisplay();
}


/* --------- Operator inputs --------- */
bool ControllerTask::isModeButtonPressed() {
    Serial.println("isModeButtonPressed inizio");

    pBtn->sync();
    // Serial.println("PostSync");
    return pBtn->isClickedAndReset();
}

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

int ControllerTask::percentToServoAngle(int percent) const {
    return map(percent, 0, 100, 0, 90);
}

void ControllerTask::checkSystemState() {
    // entrami sono check and reset del flag
    Serial.println("checkSystemState");
    // isModeButtonPressed();
    if (isModeButtonPressed()) {
    // if (1) {
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
    // 2) Manual pot -> valve opening (keep your rule: only if CONNECTED)
    if (systemState == SystemState::MANUAL_LOCAL) {
        setValveOpening(pController->getPotentiometerPosition());
    } else if (connectivityState == ConnectivityState::CONNECTED) { // se disconnesso chiudo
        setValveOpening(0);
    } else {
        setValveOpening(pController->getValveOpening());
    }
    applyValveToServo();
}

void ControllerTask::applyValveToServo() {
    // Safety: if UNCONNECTED, force closed
    int effectivePercent = (connectivityState == ConnectivityState::UNCONNECTED) ? 0 : valveOpening;
    int angle = percentToServoAngle(effectivePercent);
    pServo->setPosition(angle);
}



void ControllerTask::setSystemState(SystemState systemMode) {
    systemState = systemMode;
}

SystemState ControllerTask::getSystemState() const {
    return systemState;
}

void ControllerTask::setConnectivity(ConnectivityState s) {
    connectivityState = s;

    // Safety: if UNCONNECTED, close valve
    if (connectivityState == ConnectivityState::UNCONNECTED) {
        setValveOpening(0);
    }
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
    valveOpening = pController->clampPercent(percent);
}

int ControllerTask::getValveOpening() const {
    return valveOpening;
}