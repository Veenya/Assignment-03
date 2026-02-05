#include "Arduino.h"

#include "Controller.h"
#include "config.h"
#include "kernel/Logger.h"

bool SVILUPPO = true;

Controller::Controller(HWPlatform* hw) : pHW(hw), waterState(WaterState::Low) {}

void Controller::init() {
    L1isOn = false;
    L2isOn = false;

    if (SVILUPPO) {
        Serial.println("Led TEST");

        pHW->getL1()->switchOn();
        pHW->getL2()->switchOn();

        delay(1000);

        pHW->getL1()->switchOff();
        pHW->getL2()->switchOff();
    }
    Serial.println("Leds are READY");
}

/* --------- Stato acqua --------- */

void Controller::setWaterState(WaterState state) {
    this->waterState = state;
}

void Controller::setMQTTState(MQTTState state) {
    this->mqttState = state;
}

/* --------- Letture sensori --------- */

float Controller::getDistance() {
    auto sonar = pHW->getDDD();
    if (!sonar) {
        Serial.println("Chiamata Controller::getDistance NO SONAR");
        return SONAR_NO_OBJ_DETECTED;
    }
    return sonar->getDistance();
}


/* --------- Stato acqua --------- */

WaterState Controller::getWaterState() {
    return this->waterState;
}

MQTTState Controller::getMQTTState() {
    return this->mqttState;
}

HWPlatform* Controller::getHWPlatform() {
    return this->pHW;
}

void Controller::setL1On() {
    this->L1isOn = true;
}
void Controller::setL2On() {
    this->L2isOn = true;
}

void Controller::setL1Off() {
    this->L1isOn = false;
}

void Controller::setL2Off() {
    this->L2isOn = false;
}
void Controller::setWaterDistance(float distance) {
    this->waterDistance = distance;
}

float Controller::getWaterDistance() {
    return this->waterDistance;
}

void Controller::sync() {
    // manageAlarm();
    // manageLeds();
}
