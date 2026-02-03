#include <Arduino.h>
#include "model/TankSystem.h"
#include "TankTask.h"

// Set false to disable debug prints
static bool DEBUG = true;

TankTask::TankTask(HWPlatform* hw)
    : pHW(hw),
      mode(SystemMode::AUTOMATIC),
      connectivity(ConnectivityState::UNCONNECTED),
      waterLevel(0.0f),
      valveOpening(0),
      lastButtonState(false) {}

void TankSystem::init() {
    mode = SystemMode::AUTOMATIC;
    connectivity = ConnectivityState::UNCONNECTED;
    waterLevel = 0.0f;
    valveOpening = 0;
    lastButtonState = false;

    refreshOutputs();

    if (DEBUG) {
        Serial.println("[WCS] TankTask initialized (AUTO, UNCONNECTED, valve=0%)");
    }
}

/* --------- Mode & connectivity --------- */

void TankTask::setMode(SystemMode m) {
    mode = m;
}

SystemMode TankTask::getMode() const {
    return mode;
}

void TankSystem::toggleMode() {
    mode = (mode == SystemMode::AUTOMATIC) ? SystemMode::MANUAL : SystemMode::AUTOMATIC;

    if (DEBUG) {
        Serial.print("[WCS] Mode toggled -> ");
        Serial.println(mode == SystemMode::MANUAL ? "MANUAL" : "AUTOMATIC");
    }
}

void TankTask::setConnectivity(ConnectivityState s) {
    connectivity = s;

    // Safety: if UNCONNECTED, close valve
    if (connectivity == ConnectivityState::UNCONNECTED) {
        setValveOpening(0);
    }
}

ConnectivityState TankTask::getConnectivity() const {
    return connectivity;
}

bool TankTask::isManual() const {
    return mode == SystemMode::MANUAL;
}

bool TankTask::isUnconnected() const {
    return connectivity == ConnectivityState::UNCONNECTED;
}

/* --------- Water level (optional for LCD) --------- */

void TankTask::setWaterLevel(float wl) {
    waterLevel = wl;
}

float TankTask::getWaterLevel() const {
    return waterLevel;
}

/* --------- Valve control --------- */

void TankTask::setValveOpening(int percent) {
    valveOpening = clampPercent(percent);
}

int TankTask::getValveOpening() const {
    return valveOpening;
}

void TankTask::applyValveToServo() {
    auto servo = pHW->getValveMotor();
    if (!servo) return;

    // Safety: if UNCONNECTED, force closed
    int effectivePercent = (connectivity == ConnectivityState::UNCONNECTED) ? 0 : valveOpening;
    int angle = percentToServoAngle(effectivePercent);
    servo->setPosition(angle);
}

/* --------- Operator inputs --------- */

bool TankTask::isModeButtonPressed() {
    auto btn = pHW->getToggleButton();
    if (!btn) return false;

    btn->sync();
    bool current = btn->isPressed();

    // rising edge detect
    bool pressedEdge = (current && !lastButtonState);
    lastButtonState = current;
    return pressedEdge;
}

int TankTask::readManualValveFromPot() {
    auto pot = pHW->getPotentiometer();
    if (!pot) return 0;

    pot->sync();

    // Your PotentiometerImpl::position() should already be 0..100
    int percent = (int)pot->position();
    return clampPercent(percent);
}

/* --------- Outputs: LCD --------- */

void TankTask::updateDisplay() {
    auto lcd = pHW->getLcd();
    if (!lcd) return;

    lcd->setCursor(0, 0);
    lcd->print("V:");
    lcd->print(getValveOpening());
    lcd->print("% ");

    if (connectivity == ConnectivityState::UNCONNECTED) {
        lcd->print("UNCONN ");
    } else {
        lcd->print(mode == SystemMode::MANUAL ? "MANUAL " : "AUTO   ");
    }

    lcd->setCursor(0, 1);
    lcd->print("WL:");
    lcd->print(waterLevel, 1);
    lcd->print("       ");
}

void TankTask::refreshOutputs() {
    applyValveToServo();
    updateDisplay();
}

/* --------- helpers --------- */

int TankTask::clampPercent(int v) const {
    if (v < 0) return 0;
    if (v > 100) return 100;
    return v;
}

int TankTask::percentToServoAngle(int percent) const {
    return map(percent, 0, 100, 0, 90);
}
