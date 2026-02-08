package org.mqttserver.presentation;

public enum ArduinoConnectionStatus {
    CONNECTED,
    KO;

    @Override
    public String toString() {
        return switch (this) {
            case CONNECTED -> "CONNECTED";
            case KO -> "KO";
        };
    }
}
