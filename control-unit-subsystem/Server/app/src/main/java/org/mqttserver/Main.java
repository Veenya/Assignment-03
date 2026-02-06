package org.mqttserver;

import io.vertx.core.Vertx;
import org.mqttserver.serial.SerialCommChannelImpl;
import org.mqttserver.serial.SerialScannerImpl;
import org.mqttserver.services.http.DataService;
import org.mqttserver.services.mqtt.RemoteBrokerClientImpl;

public class Main {

    public static void main(String[] args) throws Exception {

        // 1) MQTT client (broker online)
        RemoteBrokerClientImpl mqttClient = new RemoteBrokerClientImpl(
                "broker.mqtt-dashboard.com",
                1883,
                "java-client-demo-123"
        );
        mqttClient.start();

        // 2) HTTP service (dashboard)
        Vertx vertx = Vertx.vertx();
        int httpPort = 8050;
        vertx.deployVerticle(new DataService(httpPort, mqttClient.getSystemController()));

        // 3) SERIAL test (non blocca il resto)
        //String serialPort = new SerialScannerImpl().getConnectedPort();
        String serialPort = "COM5";
        System.out.println("Using serial port: " + serialPort);

        SerialCommChannelImpl serialComm = new SerialCommChannelImpl(serialPort, 9600);

        // Thread che manda PING ogni 3s
        Thread tx = new Thread(() -> {
            while (true) {
                serialComm.sendMessageToArduino("PING");
                try { Thread.sleep(3000); } catch (InterruptedException ignored) {}
            }
        });
        tx.setDaemon(true);
        tx.start();

        // Thread che legge e stampa tutto ciò che arriva
        Thread rx = new Thread(() -> {
            while (true) {
                try {
                    if (serialComm.isMsgAvailable()) {
                        String msg = serialComm.receiveMessageFromArduino();
                        if (msg != null) System.out.println("SERIAL RX: " + msg);
                    }
                    Thread.sleep(20);
                } catch (Exception e) {
                    System.err.println("Serial RX error: " + e.getMessage());
                }
            }
        });
        rx.setDaemon(true);
        rx.start();

        System.out.println("System started: HTTP on " + httpPort + ", MQTT connected, Serial test running");
    }
}
