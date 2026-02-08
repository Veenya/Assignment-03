package org.mqttserver.presentation;

public enum SystemStatus {
    MANUAL_LOCAL,
    MANUAL_REMOTE,
    AUTOMATIC;


    @Override
    public String toString() {
        return switch (this) {
            case MANUAL_LOCAL -> "MANUAL_LOCAL";
            case MANUAL_REMOTE -> "MANUAL_REMOTE";
            case AUTOMATIC -> "AUTO";
        };
    }
}

