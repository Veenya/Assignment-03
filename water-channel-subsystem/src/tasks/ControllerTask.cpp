#include <Arduino.h>
#include "model/Controller.h"
#include "ControllerTask.h"

// Set false to disable debug prints
static bool DEBUG = true;

ControllerTask::ControllerTask(Controller* pController)
    : systemState(SystemState::AUTOMATIC),
      connectivityState(ConnectivityState::UNCONNECTED),
      waterLevel(0.0f),
      valveOpening(0),
      lastButtonState(false),
      pController(pController)
      {}


void ControllerTask::tick() {
    this->systemState = pController->getSystemState();
    this->connectivityState = pController->getConnectivityState();
    // TODO fare i manage() necessari


    // 1) Mode toggle (debounced, toggle once per press) -> like HWPlatform::test()
    if (isModeButtonPressed()) {
        toggleMode();
    }

    // 2) Manual pot -> valve opening (keep your rule: only if CONNECTED)
    if (systemState == SystemState::MANUAL && connectivityState == ConnectivityState::CONNECTED) {
        int potPercent = readManualValveFromPot();
        setValveOpening(potPercent);
    }

    // 3) Apply outputs
    applyValveToServo();
    updateDisplay();

    // 4) Serial debug similar to test()
    if (DEBUG) {
        int potPct = 0;
        auto pot = pHW->getPotentiometer();
        if (pot) {
            pot->sync();
            potPct = (int)pot->getPosition();
        }

        Serial.print("MODE=");
        Serial.print(systemState == SystemState::AUTOMATIC ? "AUTO" : "MANUAL");
        Serial.print("  POT=");
        Serial.print(potPct);
        Serial.print("%  VALVE=");
        Serial.print(getValveOpening());
        Serial.println("%");
    }
}

void ControllerTask::test() {
    //systemState;              // 0=AUTO, 1=MANUAL (persists)
    static int lastReading = HIGH;    // because INPUT_PULLUP: released=HIGH
    static unsigned long lastDebounceTime = 0;
    const unsigned long debounceMs = 40;

    // Read raw button (pressed = LOW)
    int reading = digitalRead(BUTTON_PIN);

    // Debounce: detect stable change
    if (reading != lastReading) {
        lastDebounceTime = millis();
        lastReading = reading;
    }

    // When stable for debounceMs and currently pressed -> toggle once
    static int stableState = HIGH;
    if ((millis() - lastDebounceTime) > debounceMs) {
        if (reading != stableState) {
            stableState = reading;

            if (stableState == LOW) {     // pressed event
                // toggle
                toggleMode();
            }
        }
    }

    // Pot (your position() should already be 0..100)
    int pct = readManualValveFromPot();
    
    //pMotor->setPosition(pct);
    setValveOpening(pct);
    // servo->setPosition(angle);

    refreshOutputs();
}

/* --------- Mode & connectivityState --------- */

void ControllerTask::setMode(SystemState m) {
    systemState = m;
}

SystemState ControllerTask::getMode() const {
    return systemState;
}

void ControllerTask::toggleMode() {
    systemState = (systemState == SystemState::AUTOMATIC) ? SystemState::MANUAL : SystemState::AUTOMATIC;

    if (DEBUG) {
        Serial.print("[WCS] Mode toggled -> ");
        Serial.println(systemState == SystemState::MANUAL ? "MANUAL" : "AUTOMATIC");
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
    return systemState == SystemState::MANUAL;
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

// Debounced pressed-event (toggle once per press), same logic as HWPlatform::test()
bool ControllerTask::isModeButtonPressed() {
    auto btn = pHW->getToggleButton();
    if (!btn) return false;

    // Update button internal state
    btn->sync();

    // We want: reading == LOW when pressed, HIGH when released (INPUT_PULLUP semantics)
    // Assumption: btn->isPressed() == true when physically pressed.
    // If in your implementation it's inverted, change this line accordingly.
    int reading = btn->isPressed() ? LOW : HIGH;

    // Debounce: detect change



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
        lcd->print(systemState == SystemState::MANUAL ? "MANUAL " : "AUTO   ");
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
