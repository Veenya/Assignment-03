package org.mqttserver.presentation;

public enum ValveStatus {
    AUTO_CLOSED,  
    AUTO_OPEN_50,
    AUTO_OPEN_100;

    @Override
    public String toString() {
        return switch (this) {
            case AUTO_CLOSED -> "AUTO_CLOSED";
            case AUTO_OPEN_50 -> "KAUTO_OPEN_50O";
            case AUTO_OPEN_100 -> "KAUTO_OPEN_50O";
        };
    }

    public int valvePercent() {
        return switch (this) {
            case AUTO_CLOSED -> 0;
            case AUTO_OPEN_50 -> 50;
            case AUTO_OPEN_100 -> 100;
            default -> -1; // not applicable (manual/unconnected/not available/invalid)
        };
    }
}
