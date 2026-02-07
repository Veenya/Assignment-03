package org.mqttserver;

import io.vertx.core.Vertx;

import org.mqttserver.policy.SystemControllerImpl;
import org.mqttserver.policy.SystemController;
import org.mqttserver.serial.SerialCommChannelImpl;
import org.mqttserver.serial.SerialScannerImpl;
import org.mqttserver.services.http.DataService;
import org.mqttserver.services.mqtt.RemoteBrokerClientImpl;

//import com.google.gson.JsonObject;
import io.vertx.core.json.JsonObject;

/*
Avvia tre canali in parallelo:
    - MQTT
    - HTTP
    - Seriale
e li collega tramite un istanza di SystemController (controller)
*/

public class Main {

    public static void main(String[] args) throws Exception {


        String clientId = "java-client-" + System.currentTimeMillis();

        // 1) MQTT client (broker online)
        RemoteBrokerClientImpl mqttClient = new RemoteBrokerClientImpl(
                "broker.mqtt-dashboard.com",
                1883,
                clientId
        );
        mqttClient.start();

        SystemController controller = mqttClient.getSystemController();

        // 2) HTTP service (dashboard)
        Vertx vertx = Vertx.vertx();
        int httpPort = 8050;
        vertx.deployVerticle(new DataService(httpPort, controller));

        // 3) SERIAL test (non blocca il resto)
        // TODO: debug serial scanner...
        //String serialPort = new SerialScannerImpl().getConnectedPort();
        String serialPort = "COM5";
        System.out.println("Using serial port: " + serialPort);

        SerialCommChannelImpl serialComm = new SerialCommChannelImpl(serialPort, 9600);
        

        // Thread che manda PING ogni 3s
        Thread tx = new Thread(() -> {
            while (true) {
                //serialComm.sendMessageToArduino("PING");
                try { 
                    JsonObject cmd = new JsonObject()
                    .put("isManual", controller.getIsManual())
                    .put("status", controller.getStatus().toString())
                    .put("valveValue", controller.getValveValue());

                    serialComm.sendMessageToArduino(cmd.encode());
                    Thread.sleep(3000); 
                } catch (InterruptedException ignored) {}
            }
        });
        tx.setDaemon(true);
        tx.start();

        // Thread che legge e stampa tutto ciò che arriva
        // TODO: salvare la modalita' (se arduino la cambia cambiala)
        // TODO: se siamo in manual, prendi per vero il comando da arduino
        Thread rx = new Thread(() -> {
            while (true) {
                try {
                    if (serialComm.isMsgAvailable()) {
                        String msg = serialComm.receiveMessageFromArduino();
                        if (msg == null || msg.isBlank()) continue;
                        if (msg != null) System.out.println("SERIAL RX: " + msg);

                        // Arduino deve mandare JSON valido e terminare con \n.1
                        JsonObject rep = new JsonObject(msg); // msg deve essere JSON valido

                        // valveValue riportato da Arduino (feedback)
                        if (rep.containsKey("valveValue")) {
                            Integer reportedValve = rep.getInteger("valveValue");
                            if (reportedValve != null) {

                                if (controller.getIsManual()) {
                                    // 🔴 MANUAL: Arduino comanda davvero
                                    controller.setValveValueFromDashboard(reportedValve);
                                    System.out.println("Valve set from Arduino (MANUAL): " + reportedValve);
                                } else {
                                    // 🟡 AUTO: solo verifica
                                    int expectedValve = controller.getValveValue();
                                    if (reportedValve != expectedValve) {
                                        System.err.println(
                                            "Valve mismatch in AUTO: reported=" + reportedValve +
                                            " expected=" + expectedValve
                                        );
                                    }
                                }
                            }
                        }

                        // mode
                        //TODO: meglio se non cambiamo qui lo stato...
                        if (rep.containsKey("mode")) {
                            String mode = rep.getString("mode");
                            if (mode != null) {
                                controller.setIsManual(!"auto".equalsIgnoreCase(mode));
                                //TODO: meglio quello stoott
                                // if (mode != null) controller.setArduinoModeReported(mode);
                                System.out.println("Arduino mode: " + rep.getString("mode"));
                            } 
                            
                        }
                        
                    }
                    controller.updatePolicy();
                    controller.updateConnectivity();
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
