// #include "SonarTask.h"
// #include <Arduino.h>
// #include "config.h"
// #include "kernel/Logger.h"

// bool DEBUG_LEVEL = true;

// SonarTask::SonarTask(Hangar* pHangar, UserPanel* pUserPanel) : pHangar(pHangar) {
//     setState(HangarState::NORMAL);
// }

// /*
//  * Rainwater level monitoring
//  * When the rainwater level exceeds the level L1 (but below L2, with L1 < L2) for more than T1 time, 
//  * the water channel is opened at 50% until the rainwater level is below L1.
//  * If the the rainwater level exceeds the level L2, the water channel is immediately opened at 100%, until the value is below L2.
//  * 
//  * BELOW_L1 -> LOW
//  * ABOVE_L1_FORLESSTHAN_T1 -> MEDIUM
//  * ABOVE_L1_FORMORETHAN_T1 -> HIGH
//  * ABOVE_L2 -> VERY_HIGH
// */

// void SonarTask::tick() {
//     pHangar->sync();

//     //float temp = pHangar->getTemperature();
//     float level = pMonitor->getWaterLevel();
//     long time = millis();

//     if (!DEBUG_LEVEL) {
//         switch (state) {
//             case WaterState::LOW: {
//                 pHangar->setHangarState(WaterState::LOW);

//                 if (level > L1) {
//                     setState(WaterState::PRE_ALARM);
//                     if (time > T1) {
//                         setWaterState(WaterState::HIGH)
//                     } 
//                     setWaterState(WaterState::MEDIUM)
//                 }
//                 break;
//             }

//             case WaterState::MEDIUM {}
//             case WaterState::HIGH {}
//             case WaterState::VERY_HIGH {}

            
//         }  // end switch
//     }
// }

// void TemperatureTask::setState(HangarState state) {
//     this->state = state;
//     stateTimestamp = millis();
//     justEntered = true;
// }

// long TemperatureTask::elapsedTimeInState() {
//     return millis() - stateTimestamp;
// }