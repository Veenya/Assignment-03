#include "Arduino.h"

#include "Monitor.h"
#include "config.h"
#include "kernel/Logger.h"

bool SVILUPPO = true;

Monitor::Monitor(HWPlatform* hw) : pHW(hw), waterState(WaterState::Low) {}

void Monitor::init() {
    L1isOn = false;
    L2isOn = false;
    alarmRaised = false;

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

void Monitor::setWaterState(WaterState state) {
    this->waterState = state;
}

/* --------- Letture sensori --------- */

float Monitor::getDistance() {
    auto sonar = pHW->getDDD();
    if (!sonar) {
        return SONAR_NO_OBJ_DETECTED;
    }
    return sonar->getDistance();
}


/* --------- Stato acqua --------- */

WaterState Monitor::getWaterState() {
    return this->waterState;
}

MQTTState Monitor::getMQTTState() {
    return this->mqttState;
}

void Monitor::setL1On() {
    this->L1isOn = true;
}
void Monitor::setL2On() {
    this->L2isOn = true;
}

void Monitor::setL1Off() {
    this->L1isOn = false;
}

void Monitor::setL2Off() {
    this->L2isOn = false;
}


void Monitor::manageLeds() {
    // Led1 Verde
    if (pHW->getMQTTState() == MQTTState::CONNECTED) {
        pHW->getL1()->switchOn();
        pHW->getL2()->switchOff();
    } else {
        pHW->getL2()->switchOn();
        pHW->getL1()->switchOff();
    }

}

// void Monitor::manageDoor() {
//     switch (doorState) {
//         case DoorState::OPEN:
//             Logger.log(F("[DO] Door is Open"));
//             break;
//         case DoorState::OPENING:
//             this->openDoor();
//             Logger.log(F("[DO] opening door"));
//             break;
//         case DoorState::CLOSED:
//             Logger.log(F("[DO] Door is Closed"));
//             break;
//         case DoorState::CLOSING:
//             this->closeDoor();
//             Logger.log(F("[DO] closing door"));
//             break;
//         default:
//             break;
//     }
// }



// void Monitor::raiseAlarm() {
//     this->alarmRaised = true;
// }
// void Monitor::resetAlarm() {
//     this->alarmRaised = false;
// }
// void Monitor::manageAlarm() {
//     if (alarmRaised && droneState != DroneState::TAKING_OFF && droneState != DroneState::LANDING &&
//         droneState != DroneState::WAITING) {
//         hangarState = HangarState::ALARM;
//     } else {
//         hangarState = HangarState::NORMAL;
//     }
// }




void Monitor::sync() {
    // manageAlarm();
    manageLeds();
}
