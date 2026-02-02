package org.mqttserver.policy;

import org.mqttserver.presentation.Status;

public interface ChannelControllerManager {
    void sendMessageToArduino(Status status);
    void sendMessageToArduino(int valveValue);
    String receiveDataFromArduino() throws InterruptedException;
}
