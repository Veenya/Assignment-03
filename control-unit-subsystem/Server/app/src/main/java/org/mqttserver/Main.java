package org.mqttserver;

import io.vertx.core.Vertx;
import org.mqttserver.policy.ChannelControllerManager;
import org.mqttserver.policy.ChannelControllerManagerImpl;
import org.mqttserver.presentation.Status;
import org.mqttserver.services.http.DataService;
import org.mqttserver.services.mqtt.Broker;
import org.mqttserver.services.mqtt.BrokerImpl;

public class Main {
  public static void main(String[] args) throws Exception {

    System.out.println("Welcome to Smart Tank Monitoring System server ...");

    // Start mqtt broker/server (cus <-> tms)
    Broker broker = new BrokerImpl();
    broker.initialize(broker.getMqttServer());

    // Start http service (cus <-> dbs)
    Vertx vertx = Vertx.vertx();
    vertx.deployVerticle(new DataService(8080, broker));
    System.out.println("HTTP dashboard API on port 8050");

    // Serial manager (cus <-> wcs)
    ChannelControllerManager channelControllerManager = new ChannelControllerManagerImpl(broker);

    // Periodic control loop every 400 ms
    vertx.setPeriodic(400, id -> {
      try {
        Status st = broker.getSystemController().getStatus();

        // If UNCONNECTED: notify WCS (and optionally force valve safe)
        if (st == Status.UNCONNECTED) {
          // force valve to safe position 0
          channelControllerManager.sendMessageToArduino(0);
          return;
        }

         // MANUAL: operator controls valveValue
        if (broker.getSystemController().getIsManual() || st == Status.MANUAL) {
          int manualValve = broker.getSystemController().getValveValue();
          channelControllerManager.sendMessageToArduino(manualValve);
          return;
        }

        // AUTOMATIC: send commanded valve percent derived from policy (0/50/100)
        int commandedValve = broker.getSystemController().getValveValue();
        channelControllerManager.sendMessageToArduino(commandedValve);

        // read back valve from Arduino and validate
        String msg = channelControllerManager.receiveDataFromArduino();
        if (msg != null && !msg.isBlank()) {
                    broker.getSystemController().checkValveValue(msg, broker);
                }
      } catch (Exception e) {
        e.printStackTrace();
      }
    });
  }
}
