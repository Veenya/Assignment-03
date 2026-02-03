#include "InputTask.h"
#include "Arduino.h"
#include "config.h"

InputTask::InputTask(TankSystem* pTankSystem, UserPanel* pUserPanel, Button* button, PotentiometerImpl* pot)
    : pTankSystem(pTankSystem) {
        //TODO: metti qualcosa?
        Serial.println("Siamo in InputTask");
    }

void InputTask::tick() {
    pTankSystem->sync();
}



void InputTask::handleButtonPress()
{
    if (button->isPressed() && !pressed)
    {
        unsigned long currentMillis = millis();
        if (currentMillis - lastButtonPress > DEBOUNCE_DELAY)
        {
            lastButtonPress = currentMillis;
            toggleMode();
        }
    }
}
void InputTask::toggleMode() {
    SystemMode current = pTankSystem->getMode();
    SystemMode next = (current == SystemMode::MANUAL)
                        ? SystemMode::AUTOMATIC
                        : SystemMode::MANUAL;

    pTankSystem->setMode(next);
}

void InputTask::handleModeChange() {
    if (pTankSystem->isManual()) {
        handleManualMode();
    } else {
        handleAutomaticMode();
    }
}


void InputTask::handleManualMode()
{
    if (pot->moved())
    {
        pTankSystem->valveOpening = pot->position();
    }
}

void InputTask::handleAutomaticMode()
{
    //String receivedContent = messageReceiver.getReceivedContent();
    //String systemState = jsonProcessor.getSystemState(receivedContent);
    //int valveValue = jsonProcessor.getValveValue(receivedContent);
    //waterChannelController->activePosition = valveValue;
    // motor->setPosition(valveValue); TODO: fix this
    // logger("System state: " + systemState);
    // logger("Valve value: " + String(valveValue));
}

//void InputTask::updatePressedState()
//{
    //pressed = button->isPressed();
//}