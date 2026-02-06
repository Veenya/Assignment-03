
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
      valveOpeningPercent(0),
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
    // - if MANUAL_LOCAL -> read pot -> set valveOpening
    // - apply servo + update lcd

    // 1) Button toggle (edge detect)
    bool pressed = isModeButtonPressed();
    if (pressed) {
        if (systemState == SystemState::AUTOMATIC) {
            systemState = SystemState::MANUAL_LOCAL;
            if (DEBUG) {
                Serial.println("[WCS] Mode toggled -> MANUAL_LOCAL");
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
    if (systemState == SystemState::MANUAL_LOCAL && connectivityState == ConnectivityState::CONNECTED) {
        int potPercent = getValveOpening();
        setValveOpening(potPercent);
        // displayOpeningLevel(potPercent);
    }

    // 3) Apply outputs
    applyValveToServo();
    updateDisplay();
}

/* --------- Mode & connectivityState --------- */

SystemState Controller::getSystemState() {
    return systemState;
}

void Controller::toggleMode() {
    if (systemState == SystemState::AUTOMATIC) {
        systemState = SystemState::MANUAL_LOCAL;
        if (DEBUG) {
            Serial.println("[WCS] Mode toggled -> MANUAL_LOCAL");
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
    auto servo = pHW->getMotor();
    if (!servo) {
        if (DEBUG)
            Serial.println("[WCS] No valve servo/motor");
        return;
    }

    // If UNCONNECTED, force closed (spec safety)
    int effectivePercent = (connectivityState == ConnectivityState::UNCONNECTED) ? 0 : valveOpeningPercent;
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

float Controller::getPotentiometerPosition() {
    return this->potentiometerPosition;
}

int Controller::getValveOpening() {
    return clampPercent(map(potentiometerPosition, 0, 1023, 0, 100));
}

/* --------- Outputs: LCD --------- */

void Controller::updateDisplay() {
    auto lcd = pHW->getLcd();
    if (!lcd)
        return;

    lcd->setCursor(0, 0); // Avoid heavy flicker: don’t clear constantly if you update frequently.

    // Row 0: valve + systemState
    lcd->print("V:");
    lcd->print(valveOpeningPercent);
    lcd->print("% ");

    if (connectivityState == ConnectivityState::UNCONNECTED) {
        lcd->print("UNCONN");
    } else if (systemState == SystemState::MANUAL_LOCAL) {
        lcd->print("MANUAL_LOCAL");
    } else if (systemState == SystemState::MANUAL_REMOTE) {
        lcd->print("MANUAL_REMOTE");
    } else if (systemState == SystemState::AUTOMATIC) {
        lcd->print("AUTOMATIC");
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
