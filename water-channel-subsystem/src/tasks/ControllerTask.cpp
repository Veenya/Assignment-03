#include <Arduino.h>
#include "model/Controller.h"
#include "ControllerTask.h"

// Set false to disable debug prints
static bool DEBUG = true;

ControllerTask::ControllerTask(Controller* pController)
    : 
    pController(pController),
    systemState(SystemState::AUTOMATIC),
    connectivityState(ConnectivityState::UNCONNECTED),
    waterLevel(0.0f),
    valveOpening(0),
    lastButtonState(false)
    {}


void ControllerTask::tick() {
    this->systemState = pController->getSystemState();
    this->connectivityState = pController->getConnectivityState();
    // TODO fare i manage() necessari


    // 1) Mode toggle (debounced, toggle once per press)
    if (isModeButtonPressed()) {
        toggleMode();
    }

    // 2) Manual pot -> valve opening (keep your rule: only if CONNECTED)
    if (systemState == SystemState::MANUAL_LOCAL && connectivityState == ConnectivityState::CONNECTED) {
        int potPercent = readManualValveFromPot();
        setValveOpening(potPercent);
    }

    // 3) Apply outputs
    applyValveToServo();
    updateDisplay();

    
    if (DEBUG) {
        int potPct = 0;
        auto pot = pHW->getPotentiometer();
        if (pot) {
            pot->sync();
            potPct = (int)pot->getPosition();
        }

        Serial.print("MODE=");
        Serial.print(systemState == SystemState::AUTOMATIC ? "AUTO" : "MANUAL_LOCAL");
        Serial.print("  POT=");
        Serial.print(potPct);
        Serial.print("%  VALVE=");
        Serial.print(getValveOpening());
        Serial.println("%");
    }
}


/* --------- Mode & connectivityState --------- */

void ControllerTask::setMode(SystemState m) {
    systemState = m;
}

SystemState ControllerTask::getMode() const {
    return systemState;
}

void ControllerTask::toggleMode() {
    systemState = (systemState == SystemState::AUTOMATIC) ? SystemState::MANUAL_LOCAL : SystemState::AUTOMATIC;

    if (DEBUG) {
        Serial.print("[WCS] Mode toggled -> ");
        Serial.println(systemState == SystemState::MANUAL_LOCAL ? "MANUAL_LOCAL" : "AUTOMATIC");
    }
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

void ControllerTask::setWaterLevel(float wl) {
    waterLevel = wl;
}

float ControllerTask::getWaterLevel() const {
    return waterLevel;
}

/* --------- Valve control --------- */

void ControllerTask::setValveOpening(int percent) {
    valveOpening = clampPercent(percent);
}

int ControllerTask::getValveOpening() const {
    return valveOpening;
}

void ControllerTask::applyValveToServo() {
    auto servo = pHW->getMotor();
    if (!servo) return;

    // Safety: if UNCONNECTED, force closed
    int effectivePercent = (connectivityState == ConnectivityState::UNCONNECTED) ? 0 : valveOpening;
    int angle = percentToServoAngle(effectivePercent);
    servo->setPosition(angle);
}

/* --------- Operator inputs --------- */

// Debounced pressed-event (toggle once per press)
bool ControllerTask::isModeButtonPressed() {
    auto btn = pHW->getToggleButton();
    if (!btn) return false;

    // Update button internal state
    btn->sync();

    return false;
}

int ControllerTask::readManualValveFromPot() {
    auto pot = pHW->getPotentiometer();
    if (!pot) return 0;

    pot->sync();

    // Your PotentiometerImpl::position() should already be 0..100
    int percent = (int)pot->getPosition();
    return clampPercent(percent);
}

/* --------- Outputs: LCD --------- */

void ControllerTask::updateDisplay() {
    auto lcd = pHW->getLcd();
    if (!lcd) return;

    lcd->setCursor(0, 0);
    lcd->print("V:");
    lcd->print(getValveOpening());
    lcd->print("% ");

    if (connectivityState == ConnectivityState::UNCONNECTED) {
        lcd->print("UNCONN ");
    } else {
        lcd->print(systemState == SystemState::MANUAL_LOCAL ? "MANUAL_LOCAL " : "AUTO   ");
    }

    lcd->setCursor(0, 1);
    lcd->print("WL:");
    lcd->print(waterLevel, 1);
    lcd->print("       ");
}

void ControllerTask::refreshOutputs() {
    applyValveToServo();
    updateDisplay();
}

/* --------- helpers --------- */

int ControllerTask::clampPercent(int v) const {
    if (v < 0) return 0;
    if (v > 100) return 100;
    return v;
}

int ControllerTask::percentToServoAngle(int percent) const {
    return map(percent, 0, 100, 0, 90);
}
