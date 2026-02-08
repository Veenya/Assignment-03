package org.mqttserver;

import io.vertx.core.Vertx;

import org.mqttserver.policy.SystemControllerImpl;
import org.mqttserver.services.http.DataService;
import org.mqttserver.services.mqtt.RemoteBrokerClientImpl;
import org.mqttserver.serial.SerialCommChannelImpl;

import org.mqttserver.presentation.SystemStatus;

/*
Avvia tre canali in parallelo:
    - MQTT
    - HTTP
    - Seriale
e li collega tramite un istanza di SystemController (controller)
*/

public class Main {
    public static void main(String[] args) throws Exception {
        SystemControllerImpl systemController = new SystemControllerImpl();
        String lastSystemStatusReceived[] = {"MANUAL_REMOTE"};

        String clientId = "java-client-" + System.currentTimeMillis();
        

        // 1) MQTT client (broker online)
        RemoteBrokerClientImpl mqttClient = new RemoteBrokerClientImpl(
                "broker.mqtt-dashboard.com",
                1883,
                clientId,
                systemController
        );
        mqttClient.start();

        // 2) HTTP service (dashboard)
        Vertx vertx = Vertx.vertx();
        int httpPort = 8050;
        vertx.deployVerticle(new DataService(httpPort, systemController));

        // 3) SERIAL test (non blocca il resto)
        //String serialPort = new SerialScannerImpl().getConnectedPort();
        String serialPort = "COM5";
        System.out.println("Using serial port: " + serialPort);

        SerialCommChannelImpl serialComm = new SerialCommChannelImpl(serialPort, 9600);
        
        // ---- Thread dedicato: aggiorna CONNECTED/KO indipendentemente dalla seriale ----
        Thread connectivity = new Thread(() -> {
            while (true) {
                try {
                    systemController.updateConnectivity();
                    Thread.sleep(200); // 5 Hz
                } catch (InterruptedException ignored) {}
            }
        });
        connectivity.setDaemon(true);
        connectivity.setName("CONNECTIVITY");
        connectivity.start();

        // ---------- TX: invia comandi come stringhe ----------
        // Thread che manda ogni 3s:
            // "mode"
            // "valve"
            // "wl"
            // "status"
        Thread tx = new Thread(() -> {
            while (true) {
                //serialComm.sendMessageToArduino("PING");
                try { 

                    serialComm.sendMessageToArduino("CUS," + systemController.getSystemStatus().toString() + "," + systemController.getValveValue() + "," + systemController.getWl());
                    // serialComm.sendMessageToArduino("CUS," + systemController.getSystemStatus().toString() + "," + systemController.getValveValue() + "," + 88);
                    Thread.sleep(1000); 
                    
                } catch (InterruptedException ignored) {}
            }
        });
        tx.setDaemon(true);
        tx.setName("SERIAL-TX");
        tx.start();




        // ---------- RX: riceve stringhe ----------
        // Thread che legge e stampa tutto ciò che arriva
        // Arrivano:
            // "systemState"
            // "connectionState"
            // "valveValue"
        Thread rx = new Thread(() -> {
            while (true) {
                try {
                    if (serialComm.isMsgAvailable()) {
                        String msg = serialComm.receiveMessageFromArduino();
                        if (msg != null) msg = msg.trim();

                        if (msg == null || msg.isEmpty()) {
                            continue;
                        }

                        systemController.resetLastArduinoConnection();
                        System.out.println("SERIAL RX: " + msg);


                        // Caso A: formato CSV "state,conn,valve"
                        if (msg.contains(",") && msg.split(",").length == 3) {
                            
                            String[] parts = msg.split(",", 3);
                            String arMode = parts[0].trim();     //SystemState: MANUAL_LOCAL / MANUAL_REMOTE / AUTOMATIC
                            String arConn = parts[1].trim();     // COnnectionState CONNECTED / UNCONNECTED
                            String arValveStr = parts[2].trim(); // ValveValue 0..100

                            // valve feedback
                            try {
                                if (!lastSystemStatusReceived[0].equals(arMode.trim())) {   // fai cose solo se lo stato è stato aggiornato
                                    if (arMode.startsWith("MANUAL_LOCAL")) {
                                        systemController.setSystemStatus(SystemStatus.MANUAL_LOCAL);
                                        lastSystemStatusReceived[0] = "MANUAL_LOCAL";
                                        
                                    } else if (arMode.startsWith("MANUAL_REMOTE")) {
                                        // systemController.setSystemStatus(SystemStatus.MANUAL_REMOTE);
                                        lastSystemStatusReceived[0] = "MANUAL_REMOTE";
                                        
                                    } else if (arMode.startsWith("AUTO")) {
                                        // systemController.setSystemStatus(SystemStatus.AUTOMATIC);
                                        lastSystemStatusReceived[0] = "AUTO";
                                    }
                                }
                                
                                System.out.println("Mode sent from Arduino:" + arMode);

                                // if (arMode.startsWith("UNCONNECTED")) {
                                //     controller.setStatus(Status.UNCONNECTED);
                                // }

                                if (arConn != null) {
                                    System.out.println("ConnectionStatus sent from Arduino:" + arConn);
                                }
                                



                                int reportedValve = Integer.parseInt(arValveStr);

                                if (systemController.getIsManual()) {
                                    systemController.setValveValue(reportedValve);
                                    System.out.println("Valve set from Arduino (MANUAL): " + reportedValve);
                                }
                            } catch (NumberFormatException ignored) {}

                            // mode feedback (opzionale: di solito non far comandare Arduino il mode del CUS)
                            System.out.println("Arduino mode=" + arMode + " conn=" + arConn);
                        }
                        
                    }
                    systemController.updatePolicy();
                    
                    Thread.sleep(20);
                } catch (Exception e) {
                    System.err.println("SERIAL-RX error: " + e.getMessage());
                    try { Thread.sleep(200); } catch (InterruptedException ignored) {}
                }
            }
        });
        rx.setDaemon(true);
        rx.setName("SERIAL-RX");
        rx.start();

        System.out.println("System started: HTTP on " + httpPort + ", MQTT connected, Serial test running");
    }
}
