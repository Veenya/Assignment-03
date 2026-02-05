// water-channel-subsystem/src/model/UserPanel.cpp
#include "UserPanel.h"
#include <Arduino.h>
#include "config.h"

UserPanel::UserPanel(HWPlatform* pHW)
    : pHW(pHW),
      pResetButton(nullptr),
      pLcd(nullptr),
      resetPressed(false),
      prevResetPressed(false) {
    // prendi risorse hardware dalla piattaforma
    if (this->pHW) {
        pResetButton = this->pHW->getResetButton();
        pLcd = this->pHW->getLcd();
    }
}

void UserPanel::init() {
    resetPressed = false;
    prevResetPressed = false;

    if (pLcd) {
        pLcd->init();
        pLcd->backlight();
        pLcd->clear();
        pLcd->display();
    }
}

void UserPanel::turnOnDisplay() {
    if (!pLcd) return;
    pLcd->display();
    pLcd->clear();
}

void UserPanel::turnOffDisplay() {
    if (!pLcd) return;
    pLcd->noDisplay();
}

void UserPanel::displayOpeningLevel(float val) {
    if (!pLcd) return;
    pLcd->setCursor(0, 1);
    pLcd->print("                "); // 16 spazi
    pLcd->setCursor(0, 1);
    pLcd->print("VALVE:");
    pLcd->print(val, 1); // 1 decimale
    pLcd->print("%");
}

void UserPanel::displayAutomatic() {
    if (!pLcd) return;
    pLcd->setCursor(0, 0);
    pLcd->print("                "); // 16 spazi
    pLcd->setCursor(0, 0);
    pLcd->print("AUTOMATIC");
}

void UserPanel::displayManual() {
    if (!pLcd) return;
    pLcd->setCursor(0, 0);
    pLcd->print("                "); // 16 spazi
    pLcd->setCursor(0, 0);
    pLcd->print("MANUAL");
}

void UserPanel::displayUnconnected() {
    if (!pLcd) return;
    pLcd->setCursor(0, 0);
    pLcd->print("                "); // 16 spazi
    pLcd->setCursor(0, 0);
    pLcd->print("UNCONNECTED");
}

void UserPanel::prepareToSleep() {
    if (!pLcd) return;
    pLcd->noDisplay();
}

void UserPanel::resumeFromSleeping() {
    if (!pLcd) return;
    pLcd->display();
}

void UserPanel::sync() {
    if (!pResetButton) return;

    prevResetPressed = resetPressed;
    resetPressed = pResetButton->isPressed();
}

bool UserPanel::isResetPressed() const {
    return resetPressed;
}

bool UserPanel::isResetPressedEdge() const {
    // fronte di salita: ora premuto, prima no
    return (resetPressed && !prevResetPressed);
}
