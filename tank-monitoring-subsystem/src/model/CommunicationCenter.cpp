#include "CommunicationCenter.h"
#include <Arduino.h>
#include "kernel/Logger.h"
#include "kernel/MsgService.h"

CommunicationCenter::CommunicationCenter(Controller* pController) : pController(pController) {}

void CommunicationCenter::init() {
}

void CommunicationCenter::notifyNewState() {
//     this->hangarState = pController->getControllerState();
//     String hangarStateStr;
//     if (hangarState == ControllerState::ALARM) {
//         hangarStateStr = "2";
//     } else if (hangarState == ControllerState::PRE_ALARM) {
//         hangarStateStr = "1";
//     } else {  // NORMAL
//         hangarStateStr = "0";
//     }

//     droneState = pController->getDroneState();
//     String droneStateStr = "-1";
//     if (droneState == DroneState::REST) {
//         droneStateStr = "0";
//     } else if (droneState == DroneState::OPERATING) {
//         droneStateStr = "1";
//     } else {
//         droneStateStr = "2";
//     }
//     droneAbove = pController->isDroneAbove();
//     String droneAboveStr = "-1";
//     if (droneAbove) {
//         droneAboveStr = "1";
//     } else {
//         droneAboveStr = "0";
//     }

//     droneDistance = pController->getDistance();  // solitamente tra 0 e 0.2
//     // int droneDistance = 10;
//     currentTemp = pController->getTemperature();

//   // stato del hangar, stato drone, distanza drone, temperatura, drone above
//   MsgService.sendMsg(String("STATE,") + 
//                       hangarStateStr + "," + 
//                       droneStateStr + "," + 
//                       String(droneDistance).substring(0,5) + "," +  
//                       String(currentTemp).substring(0,5) + "," +
//                       String(droneAboveStr));  
}

void CommunicationCenter::sync() {
    // if (MsgService.isMsgAvailable()) {
    //     Msg* msg = MsgService.receiveMsg();
    //     if (msg != NULL) {
    //         String msgContent = msg->getContent();
    //         Logger.log("Received msg: " + msgContent);
    //         if (msgContent == "to") {  // Take off
    //             openDoorNotification = true;
    //             takeOffNotification = true;
    //         } else if (msgContent == "la") {  // Landing
    //             openDoorNotification = true;
    //             landingNotification = true;
    //         } else if (msgContent == "ao") {  // Alarm on
    //             pController->raiseAlarm();
    //         } else if (msgContent == "af") {  // Alarm off
    //             pController->resetAlarm();
    //         }
    //         delete msg;
    //     }
    // }
    // pController->getResetButton()->sync();
    // // Logger.log("Reset Alarm Pressed" + String(pController->getResetButton()->isPressed()));
    // if (pController->getResetButton()->isPressed()) {
    //     pController->resetAlarm();
    // }
}

//* OPEN DOOR
bool CommunicationCenter::checkAndResetOpenDoorRequest() {
    // bool request = this->openDoorNotification;
    // openDoorNotification = false;
    // return request;
}

