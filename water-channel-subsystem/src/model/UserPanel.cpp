#include "UserPanel.h"
#include "config.h"

UserPanel::UserPanel(HWPlatform* pHW)
    : pHW(pHW), pResetButton(nullptr), resetPressed(false), prevResetPressed(false) {
    if (pHW) {
        pResetButton = pHW->getToggleButton();
        pLcd = pHW->getLcd();
    }
}

void UserPanel::init() {
    resetPressed = false;
    prevResetPressed = false;
    pLcd->init();
    pLcd->backlight();
    pLcd->noDisplay();
    turnOnDisplay();
}

void UserPanel::turnOnDisplay() {
    pLcd->display();
    pLcd->clear();
}

void UserPanel::turnOffDisplay() {
    pLcd->noDisplay();
}

void UserPanel::displayOpeningLevel(float val) {
    pLcd->print("                "); // 16 spazi al posto di clear
    pLcd->setCursor(0, 1);
    pLcd->print("VALVE:");
    pLcd->print(val, 1); //1 decimale
    pLcd->print("%");
}

void UserPanel::displayAutomatic() {
    pLcd->print("                "); // 16 spazi al posto di clear
    pLcd->setCursor(0, 0);
    pLcd->print("AUTOMATIC"); 
}

void UserPanel::displayManualLocal() {
    pLcd->print("                "); // 16 spazi al posto di clear
    pLcd->setCursor(0, 0);
    pLcd->print("MANUAL_LOCAL)"); 
}

void UserPanel::displayManualRemote() {
    pLcd->print("                "); // 16 spazi al posto di clear
    pLcd->setCursor(0, 0);
    pLcd->print("MANUAL_REMOTE)"); 
}

void UserPanel::displayUnconnected() {
    pLcd->print("                "); // 16 spazi al posto di clear
    pLcd->setCursor(0, 0);
    pLcd->print("UNCONNECTED"); 
}

void UserPanel::displayWaterLevel(int waterLevel) {
    pLcd->setCursor(0, 1);
    pLcd->print("WL:");
    pLcd->print(waterLevel, 1);
    pLcd->print("       ");
}

void UserPanel::prepareToSleep() {
    pLcd->noDisplay();
}

void UserPanel::resumeFromSleeping() {
    pLcd->display();
}

void UserPanel::sync() {
    if (!pResetButton) {
        return;
    }

    prevResetPressed = resetPressed;
    resetPressed = pResetButton->isPressed();
}

bool UserPanel::isResetPressed() const {
    return resetPressed;
}

bool UserPanel::isResetPressedEdge() {
    // fronte di salita: ora premuto, prima no
    return (resetPressed && !prevResetPressed);
}
