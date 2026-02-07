
#include "Controller.h"
#include "Arduino.h"
#include "UserPanel.h"
#include "config.h"

// Set false to disable debug prints
bool SVILUPPO = true;

Controller::Controller(HWPlatform* hw)
    : 
    pHW(hw),
    systemState(SystemState::MANUAL_LOCAL),
    connectivityState(ConnectivityState::UNCONNECTED),
    waterLevel(0.0f),
    valveOpening(0),
    lastButtonState(false) {}

void Controller::init() {
    pServo = pHW->getMotor();
    pBtn = pHW->getButton();
    
    pServo->motorOn();
    // pServo->setPosition(0);
    moveMotor(0);
    if (SVILUPPO) {
        Serial.println("Valve TEST");
        delay(1000);
        moveMotor(90);
        delay(1000);
        moveMotor(0);
        // while (1) {
        //     delay(1000);
        //     pServo->setPosition(90);
        //     delay(1000);
        //     pServo->setPosition(0);
        // }
    }

    Serial.println("Controller initialized");
}


/* --------- Mode & connectivityState --------- */

SystemState Controller::getSystemState() {
    return systemState;
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

void Controller::setValveOpening(int valveOpening) {
    this->valveOpening = valveOpening;
}

HWPlatform* Controller::getHWPlatform() {
    return pHW;
}

void Controller::moveMotor(int angle) {
    pServo->setPosition(angle);
}

void Controller::syncButton() {
    this->pBtn->sync();
}

bool Controller::buttonCheckAndConsumeClick() {
    Serial.print("Controller::buttonCheckAndConsumeClick ");
    bool clicked = this->pBtn->checkAndConsumeClick();
    Serial.println(clicked);
    
    return clicked;
    // return this->pBtn->checkAndConsumeClick();
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

void Controller::setPotentiometerPosition(int potentiometerPosition) {
    this->potentiometerPosition = potentiometerPosition;
}; 


void Controller::applyValveToServo() {
    // If UNCONNECTED, force closed (spec safety)
    int effectivePercent = (systemState != SystemState::MANUAL_LOCAL && connectivityState == ConnectivityState::UNCONNECTED) ? 0 : valveOpening;
    int angle = percentToServoAngle(effectivePercent);
    pServo->setPosition(angle);
}

float Controller::getPotentiometerPosition() {
    return this->potentiometerPosition;
}

// per ora valveOpening e potentiometerPosition sono la stessa cosa
int Controller::getValveOpening() {
    return valveOpening;
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
