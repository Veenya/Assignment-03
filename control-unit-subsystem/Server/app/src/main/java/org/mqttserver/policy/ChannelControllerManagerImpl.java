package org.mqttserver.policy;

import org.mqttserver.presentation.*;
import org.mqttserver.serial.SerialCommChannel;
import org.mqttserver.serial.SerialCommChannelImpl;
import org.mqttserver.serial.SerialScanner;
import org.mqttserver.serial.SerialScannerImpl;

import org.mqttserver.services.mqtt.Broker;


public class ChannelControllerManagerImpl implements ChannelControllerManager {
    private Broker broker;
    private final SerialCommChannel serialCommChannel;
    private SerialScanner serialScanner = new SerialScannerImpl();

    public ChannelControllerManagerImpl() throws Exception {
        String port = serialScanner.getConnectedPort();
        this.serialCommChannel = new SerialCommChannelImpl(port, 9600);
    }

    @Override
    public void sendStatusMessageToArduino(org.mqttserver.presentation.Status status) {
        // Implementation needed
    }

    @Override
    public void sendValveMessageToArduino(int valveValue) {
        // Implementation needed
    }

    @Override
    public String receiveDataFromArduino() throws InterruptedException {
        // Implementation needed
        return "";
    }
}
