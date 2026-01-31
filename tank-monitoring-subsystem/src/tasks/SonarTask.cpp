#include "SonarTask.h"
#include <Arduino.h>
#include "config.h"
#include "kernel/Logger.h"

bool DEBUG = true;

SonarTask::SonarTask(Monitor* pMonitor) : pMonitor(pMonitor) {
    setState(WaterState::Low);
}

/*
 * Rainwater level monitoring
 * When the rainwater level exceeds the level L1 (but below L2, with L1 < L2) for more than T1 time, 
 * the water channel is opened at 50% until the rainwater level is below L1.
 * If the the rainwater level exceeds the level L2, the water channel is immediately opened at 100%, until the value is below L2.
 * 
 * BELOW_L1 -> LOW
 * ABOVE_L1_FORLESSTHAN_T1 -> MEDIUM
 * ABOVE_L1_FORMORETHAN_T1 -> HIGH
 * ABOVE_L2 -> VERY_HIGH
*/

void SonarTask::tick() {
    pMonitor->sync();

    float level = pMonitor->getDistance();

    if (!DEBUG) {
        switch (state) {
            case WaterState::Low: {
                pMonitor->setWaterState(WaterState::Low);

                if (level > L1) {
                    setState(WaterState::Medium);

                } else if (elapsedTimeInState() > TIME1) {
                        setState(WaterState::High);
                        
                }
                break;
            }

            case WaterState::Medium: {
                if (elapsedTimeInState() > TIME1) {
                    setState(WaterState::High);
                } // TODO controllare se funziona
            }

            case WaterState::High: {
                break;
            }


            
        }  // end switch
        pMonitor->setWaterState(this->state);
    }
}

void SonarTask::setState(WaterState state) {
    this->state = state;
    stateTimestamp = millis();
    justEntered = true;
}

long SonarTask::elapsedTimeInState() {
    return millis() - stateTimestamp;
}