
#include "Controller.h"
#include "Arduino.h"
#include "UserPanel.h"
#include "config.h"

// Set false to disable debug prints
static bool DEBUG = true;

Controller::Controller(HWPlatform* hw)
    : pHW(hw),
      systemState(SystemState::AUTOMATIC),
      connectivityState(ConnectivityState::UNCONNECTED),
      waterLevel(0.0f),
      valveOpening(0),
      lastButtonState(false) {}

void Controller::init() {
    // Put valve in safe state (closed)
    applyValveToServo();
    updateDisplay();

    if (DEBUG) {
        Serial.println("[WCS] Controller initialized (AUTO, UNCONNECTED, valve=0%)");
    }
}

void Controller::sync() {
    // Typical loop:
    // - read button edge -> toggle systemState locally (operator panel)
    // - if MANUAL -> read pot -> set valveOpening
    // - apply servo + update lcd

    // 1) Button toggle (edge detect)
    bool pressed = isModeButtonPressed();
    if (pressed) {
        if (systemState == SystemState::AUTOMATIC) {
            systemState = SystemState::MANUAL;
            if (DEBUG) {
                Serial.println("[WCS] Mode toggled -> MANUAL");
                // displayManual();
            }
        } else {
            systemState = SystemState::AUTOMATIC;
            if (DEBUG) {
                Serial.println("[WCS] Mode toggled -> AUTOMATIC");
                // displayAutomatic();
            }
        }
    }

    // 2) If manual and connected, valve opening follows potentiometer
    if (systemState == SystemState::MANUAL && connectivityState == ConnectivityState::CONNECTED) {
        int potPercent = readManualValveFromPot();
        setValveOpening(potPercent);
        // displayOpeningLevel(potPercent);
    }

    // 3) Apply outputs
    applyValveToServo();
    updateDisplay();
}

/* --------- Mode & connectivityState --------- */

void Controller::setMode(SystemState m) {
    systemState = m;
}

SystemState Controller::getMode() {
    return systemState;
}

void Controller::toggleMode() {
    if (systemState == SystemState::AUTOMATIC) {
        systemState = SystemState::MANUAL;
        if (DEBUG) {
            Serial.println("[WCS] Mode toggled -> MANUAL");
        }
    } else {
        systemState = SystemState::AUTOMATIC;
        if (DEBUG) {
            Serial.println("[WCS] Mode toggled -> AUTOMATIC");
        }
    }
}

void Controller::setConnectivity(ConnectivityState s) {
    connectivityState = s;

    // Safety behavior: if UNCONNECTED, close valve
    if (connectivityState == ConnectivityState::UNCONNECTED) {
        setValveOpening(0);
    }
}

ConnectivityState Controller::getConnectivity() {
    return connectivityState;
}

bool Controller::isManual() {
    return systemState == SystemState::MANUAL;
}

bool Controller::isUnconnected() {
    return connectivityState == ConnectivityState::UNCONNECTED;
}

/* --------- Water level (optional for LCD) --------- */

void Controller::setWaterLevel(float wl) {
    waterLevel = wl;
}

float Controller::getWaterLevel() {
    return waterLevel;
}

/* --------- Valve control --------- */

void Controller::setValveOpening(int percent) {
    valveOpening = clampPercent(percent);
}

int Controller::getValveOpening() {
    return valveOpening;
}

HWPlatform* Controller::getHWPlatform() {
    return pHW;
}


SystemState Controller::getSystemState() {
    return this->systemState;
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
    auto servo = pHW->getMotor();
    if (!servo) {
        if (DEBUG)
            Serial.println("[WCS] No valve servo/motor");
        return;
    }

    // If UNCONNECTED, force closed (spec safety)
    int effectivePercent = (connectivityState == ConnectivityState::UNCONNECTED) ? 0 : valveOpening;
    int angle = percentToServoAngle(effectivePercent);
    servo->setPosition(angle);
}

/* --------- Operator inputs --------- */

bool Controller::isModeButtonPressed() {
    auto btn = pHW->getToggleButton();
    if (!btn)
        return false;

    btn->sync();
    bool current = btn->isPressed();

    // rising edge detect
    bool pressedEdge = (current && !lastButtonState);
    lastButtonState = current;
    return pressedEdge;
}

int Controller::readManualValveFromPot() {
    auto pot = pHW->getPotentiometer();  // <-- rename to your HWPlatform method
    if (!pot)
        return 0;

    pot->sync();
    // assume pot returns 0..1023; map to 0..100
    int raw = pot->getPosition();
    int percent = map(raw, 0, 1023, 0, 100);
    return clampPercent(percent);
}

/* --------- Outputs: LCD --------- */

void Controller::updateDisplay() {
    auto lcd = pHW->getLcd();  // <-- rename to your HWPlatform method
    if (!lcd)
        return;

    // Avoid heavy flicker: don’t clear constantly if you update frequently.
    // If you prefer, you can clear, but it may flicker.
    lcd->setCursor(0, 0);

    // Row 0: valve + systemState
    // Example: "V: 50% AUTO"
    lcd->print("V:");
    lcd->print(getValveOpening());
    lcd->print("% ");

    if (connectivityState == ConnectivityState::UNCONNECTED) {
        lcd->print("UNCONN");
    } else {
        lcd->print(systemState == SystemState::MANUAL ? "MANUAL" : "AUTO  ");
    }

    // Row 1: WL (optional)
    lcd->setCursor(0, 1);
    lcd->print("WL:");
    lcd->print(waterLevel, 1);
    lcd->print("       ");  // padding to overwrite old chars
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
