
#include "Controller.h"
#include "Arduino.h"
#include "UserPanel.h"
#include "config.h"

// Set false to disable debug prints
static bool DEBUG = true;

Controller::Controller(HWPlatform* hw)
    : 
    pHW(hw),
    systemState(SystemState::AUTOMATIC),
    connectivityState(ConnectivityState::UNCONNECTED),
    waterLevel(0.0f),
    valveOpeningPercent(0),
    lastButtonState(false) {}

void Controller::init() {
    pServo = pHW->getMotor();
    pBtn = pHW->getButton();
    Serial.println("Controller initialized");
}


/* --------- Mode & connectivityState --------- */

SystemState Controller::getSystemState() {
    return systemState;
}

void Controller::setConnectivity(ConnectivityState connectivityState) {
    connectivityState = connectivityState;

    // Safety behavior: if UNCONNECTED, close valve
    if (connectivityState == ConnectivityState::UNCONNECTED) {
        setValveOpening(0);
    }
}

ConnectivityState Controller::getConnectivity() {
    return connectivityState;
}

bool Controller::isManualRemote() {
    return systemState == SystemState::MANUAL_REMOTE;
}

bool Controller::isManualLocal() {
    return systemState == SystemState::MANUAL_LOCAL;
}


bool Controller::isUnconnected() {
    return connectivityState == ConnectivityState::UNCONNECTED;
}

/* --------- Water level (optional for LCD) --------- */

void Controller::setWaterLevel(float waterLevel) {
    waterLevel = waterLevel;
}

float Controller::getWaterLevel() {
    return waterLevel;
}

/* --------- Valve control --------- */

void Controller::setValveOpening(int percent) {
    valveOpeningPercent = clampPercent(percent);
}

HWPlatform* Controller::getHWPlatform() {
    return pHW;
}


void Controller::setSystemState(SystemState systemState) {
    this->systemState = systemState;
}

ConnectivityState Controller::getConnectivityState(){
    return this->connectivityState;
}
void Controller::setConnectivityState(ConnectivityState connectivityState) {
    this->connectivityState = connectivityState;
}

void Controller::setPotentiometerPosition(float potentiometerPosition) {
    this->potentiometerPosition = potentiometerPosition;
}; 


void Controller::applyValveToServo() {
    // If UNCONNECTED, force closed (spec safety)
    int effectivePercent = (connectivityState == ConnectivityState::UNCONNECTED) ? 0 : valveOpeningPercent;
    int angle = percentToServoAngle(effectivePercent);
    pServo->setPosition(angle);
}

/* --------- Operator inputs --------- */

bool Controller::isModeButtonPressed() {
    pBtn->sync();
    bool current = pBtn->isPressed();

    // rising edge detect
    bool pressedEdge = (current && !lastButtonState);
    lastButtonState = current;
    return pressedEdge;
}

float Controller::getPotentiometerPosition() {
    return this->potentiometerPosition;
}

int Controller::getValveOpening() {
    return clampPercent(map(potentiometerPosition, 0, 1023, 0, 100));
}

/* --------- helpers --------- */

int Controller::clampPercent(int v) {
    if (v < 0)
        return 0;
    if (v > 100)
        return 100;
    return v;
}

int Controller::percentToServoAngle(int percent) {
    // Spec: 0% -> 0°, 100% -> 90°
    return map(percent, 0, 100, 0, 90);
}
