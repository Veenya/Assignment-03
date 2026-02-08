package org.mqttserver.presentation;

public class MessageToArduino {
    private SystemStatus systemStatus;

    private int valveValue = -1;

    public MessageToArduino(SystemStatus systemStatus) {
        this.systemStatus = systemStatus;
    }

    public MessageToArduino(int valveValue) {
        this.valveValue = valveValue;
    }

    public SystemStatus getSystemStatus() {
        return this.systemStatus;
    }

    public void setSystemStatus(SystemStatus systemStatus) {
        this.systemStatus = systemStatus;
    }

    public void setValveValue(int valveValue) {
        this.valveValue = valveValue;
    }
}
