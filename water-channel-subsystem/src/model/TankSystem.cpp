#include "Arduino.h"
#include "TankSystem.h"
#include "UserPanel.h"
#include "config.h"

// Set false to disable debug prints
static bool DEBUG = true;

TankSystem::TankSystem(HWPlatform* hw)
    : pHW(hw),
      mode(SystemMode::AUTOMATIC),
      connectivity(ConnectivityState::UNCONNECTED),
      waterLevel(0.0f),
      valveOpening(0),
      lastButtonState(false) {}

void TankSystem::init() {
    // Starting mode when booting is AUTOMATIC (spec)
    mode = SystemMode::AUTOMATIC;
    connectivity = ConnectivityState::UNCONNECTED; // until CUS sends something / heartbeat
    waterLevel = 0.0f;
    valveOpening = 0;

    // Put valve in safe state (closed)
    applyValveToServo();
    updateDisplay();

    if (DEBUG) {
        Serial.println("[WCS] TankSystem initialized (AUTO, UNCONNECTED, valve=0%)");
    }
}

void TankSystem::sync() {
    // Typical loop:
    // - read button edge -> toggle mode locally (operator panel)
    // - if MANUAL -> read pot -> set valveOpening
    // - apply servo + update lcd

    // 1) Button toggle (edge detect)
    bool pressed = isModeButtonPressed();
    if (pressed) {
        if (mode == SystemMode::AUTOMATIC) {
            mode = SystemMode::MANUAL;
            if (DEBUG) {
                Serial.println("[WCS] Mode toggled -> MANUAL");
                //displayManual();
            }
        } else {
            mode = SystemMode::AUTOMATIC;
            if (DEBUG) {
                Serial.println("[WCS] Mode toggled -> AUTOMATIC");
                //displayAutomatic();
            }
        }
    }

    // 2) If manual and connected, valve opening follows potentiometer
    if (mode == SystemMode::MANUAL && connectivity == ConnectivityState::CONNECTED) {
        int potPercent = readManualValveFromPot();
        setValveOpening(potPercent);
        //displayOpeningLevel(potPercent);
    }

    // 3) Apply outputs
    applyValveToServo();
    updateDisplay();
}

/* --------- Mode & connectivity --------- */

void TankSystem::setMode(SystemMode m) {
    mode = m;
}

SystemMode TankSystem::getMode() const {
    return mode;
}

void TankSystem::setConnectivity(ConnectivityState s) {
    connectivity = s;

    // Safety behavior: if UNCONNECTED, close valve
    if (connectivity == ConnectivityState::UNCONNECTED) {
        setValveOpening(0);
    }
}

ConnectivityState TankSystem::getConnectivity() const {
    return connectivity;
}

bool TankSystem::isManual() const {
    return mode == SystemMode::MANUAL;
}

bool TankSystem::isUnconnected() const {
    return connectivity == ConnectivityState::UNCONNECTED;
}

/* --------- Water level (optional for LCD) --------- */

void TankSystem::setWaterLevel(float wl) {
    waterLevel = wl;
}

float TankSystem::getWaterLevel() const {
    return waterLevel;
}

/* --------- Valve control --------- */

void TankSystem::setValveOpening(int percent) {
    valveOpening = clampPercent(percent);
}

int TankSystem::getValveOpening() const {
    return valveOpening;
}

void TankSystem::applyValveToServo() {
    auto servo = pHW->getValveMotor(); // <-- rename to your HWPlatform method
    if (!servo) {
        if (DEBUG) Serial.println("[WCS] No valve servo/motor");
        return;
    }

    // If UNCONNECTED, force closed (spec safety)
    int effectivePercent = (connectivity == ConnectivityState::UNCONNECTED) ? 0 : valveOpening;
    int angle = percentToServoAngle(effectivePercent);
    servo->setPosition(angle);
}

/* --------- Operator inputs --------- */

bool TankSystem::isModeButtonPressed() {
    auto btn = pHW->getModeButton(); // <-- rename to your HWPlatform method
    if (!btn) return false;

    btn->sync();
    bool current = btn->isPressed();

    // rising edge detect
    bool pressedEdge = (current && !lastButtonState);
    lastButtonState = current;
    return pressedEdge;
}

int TankSystem::readManualValveFromPot() {
    auto pot = pHW->getPotentiometer(); // <-- rename to your HWPlatform method
    if (!pot) return 0;

    pot->sync();
    // assume pot returns 0..1023; map to 0..100
    int raw = pot->getValue();
    int percent = map(raw, 0, 1023, 0, 100);
    return clampPercent(percent);
}

/* --------- Outputs: LCD --------- */

void TankSystem::updateDisplay() {
    auto lcd = pHW->getLcd(); // <-- rename to your HWPlatform method
    if (!lcd) return;

    // Avoid heavy flicker: don’t clear constantly if you update frequently.
    // If you prefer, you can clear, but it may flicker.
    lcd->setCursor(0, 0);

    // Row 0: valve + mode
    // Example: "V: 50% AUTO"
    lcd->print("V:");
    lcd->print(getValveOpening());
    lcd->print("% ");

    if (connectivity == ConnectivityState::UNCONNECTED) {
        lcd->print("UNCONN");
    } else {
        lcd->print(mode == SystemMode::MANUAL ? "MANUAL" : "AUTO  ");
    }

    // Row 1: WL (optional)
    lcd->setCursor(0, 1);
    lcd->print("WL:");
    lcd->print(waterLevel, 1);
    lcd->print("       "); // padding to overwrite old chars
}

/* --------- helpers --------- */

int TankSystem::clampPercent(int v) const {
    if (v < 0) return 0;
    if (v > 100) return 100;
    return v;
}

int TankSystem::percentToServoAngle(int percent) const {
    // Spec: 0% -> 0°, 100% -> 90°
    return map(percent, 0, 100, 0, 90);
}
