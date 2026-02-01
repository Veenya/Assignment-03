#include "Arduino.h"

#include "Controller.h"
#include "config.h"
#include "kernel/Logger.h"

bool SVILUPPO = true;

Controller::Controller(HWPlatform* hw) : pHW(hw), waterState(WaterState::Low) {}

void Controller::init() {
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

void Controller::setWaterState(WaterState state) {
    this->waterState = state;
}

/* --------- Letture sensori --------- */

float Controller::getDistance() {
    auto sonar = pHW->getDDD();
    if (!sonar) {
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


void Controller::manageLeds() {
    // Led1 Verde
    if (pHW->getMQTTState() == MQTTState::CONNECTED) {
        pHW->getL1()->switchOn();
        pHW->getL2()->switchOff();
    } else {
        pHW->getL2()->switchOn();
        pHW->getL1()->switchOff();
    }

}

// void Controller::manageDoor() {
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



// void Controller::raiseAlarm() {
//     this->alarmRaised = true;
// }
// void Controller::resetAlarm() {
//     this->alarmRaised = false;
// }
// void Controller::manageAlarm() {
//     if (alarmRaised && droneState != DroneState::TAKING_OFF && droneState != DroneState::LANDING &&
//         droneState != DroneState::WAITING) {
//         hangarState = HangarState::ALARM;
//     } else {
//         hangarState = HangarState::NORMAL;
//     }
// }




void Controller::sync() {
    // manageAlarm();
    manageLeds();
}
