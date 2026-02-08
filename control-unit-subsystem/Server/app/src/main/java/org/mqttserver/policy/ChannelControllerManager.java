package org.mqttserver.policy;

import org.mqttserver.presentation.SystemStatus;

public interface ChannelControllerManager {
    void sendMessageToArduino(SystemStatus status);
    void sendMessageToArduino(int valveValue);
    String receiveDataFromArduino() throws InterruptedException;
}
