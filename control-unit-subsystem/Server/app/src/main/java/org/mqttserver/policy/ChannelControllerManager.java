package org.mqttserver.policy;

import org.mqttserver.presentation.Status;

public interface ChannelControllerManager {
    void sendStatusMessageToArduino(Status status);
    void sendValveMessageToArduino(int valveValue);
    String receiveDataFromArduino() throws InterruptedException;
}
