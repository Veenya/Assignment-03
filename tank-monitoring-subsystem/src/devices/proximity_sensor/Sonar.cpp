#include "Sonar.h"
#include "Arduino.h"

Sonar::Sonar(int echoP, int trigP, long maxTime)
    : echoPin(echoP), trigPin(trigP), timeOut(maxTime) {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}


float Sonar::getDistance() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(3);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(5);
    digitalWrite(trigPin, LOW);

    float duration = pulseIn(echoPin, HIGH, timeOut);
    if (duration == 0) {
        Serial.println("Chiamata Sonar::getDistance NO SONAR");
        return SONAR_NO_OBJ_DETECTED;
    } else {
        distance = duration / 58.2;
        return distance;
    }
}
