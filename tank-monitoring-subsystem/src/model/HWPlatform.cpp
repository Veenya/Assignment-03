#include "HWPlatform.h"
#include <Arduino.h>

HWPlatform::HWPlatform() {
    pDdd = new Sonar(SONAR_ECHO_PIN, SONAR_TRIG_PIN, MAX_TIME_US);
    pL1 = new Led(L1_PIN);
    pL2 = new Led(L2_PIN);
    Serial.println("HWPlatform istanziata");
}

void HWPlatform::init() {
    Serial.println("HWPlatform inizializzata");
}

Led* HWPlatform::getL1() {
    return this->pL1;
}

Led* HWPlatform::getL2() {
    return this->pL2;
}

Sonar* HWPlatform::getDDD() {
    return this->pDdd;
}

