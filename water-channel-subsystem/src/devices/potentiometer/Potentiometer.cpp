#include "Potentiometer.h"

Potentiometer::Potentiometer() {}

void Potentiometer::updateSyncTime(long time) {
    lastTimeSync = time;
}

long Potentiometer::getLastSynchTime() {
    return lastTimeSync;
}