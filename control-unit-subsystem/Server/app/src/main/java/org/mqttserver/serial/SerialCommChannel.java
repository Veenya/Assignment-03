package org.mqttserver.serial;

public interface SerialCommChannel {
    void sendMessageToArduino(String message);
    String receiveMessageFromArduino() throws InterruptedException;
    boolean isMsgAvailable();
}
