#include "Potentiometer.h"

Potentiometer::Potentiometer() : lastTimeSync(0) {}

Potentiometer::~Potentiometer() {}

void Potentiometer::sync() {
    // base: non fa nulla
}

void Potentiometer::updateSyncTime(long time) {
    lastTimeSync = time;
}

long Potentiometer::getLastSynchTime() const {
    return lastTimeSync;
}
