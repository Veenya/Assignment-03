package org.mqttserver;

import io.vertx.core.Vertx;
import org.mqttserver.policy.ChannelControllerManager;
import org.mqttserver.policy.ChannelControllerManagerImpl;
import org.mqttserver.presentation.Status;
import org.mqttserver.services.http.DataService;
import org.mqttserver.services.mqtt.Broker;
import org.mqttserver.services.mqtt.BrokerImpl;

import java.util.concurrent.Callable;
import java.util.concurrent.atomic.AtomicBoolean;

public class Main {

    public static void main(String[] args) throws Exception {

        System.out.println("Welcome to Smart Tank Monitoring System server ...");

        // Start MQTT broker/server (CUS <-> TMS)
        Broker broker = new BrokerImpl();
        broker.initialize(broker.getMqttServer());

        // Start HTTP service (CUS <-> DBS)
        final int httpPort = 8050;
        Vertx vertx = Vertx.vertx();
        vertx.deployVerticle(new DataService(httpPort, broker));
        System.out.println("HTTP dashboard API on port " + httpPort);

        // Serial manager (CUS <-> WCS)
        ChannelControllerManager channelControllerManager = new ChannelControllerManagerImpl(broker);

        // Prevent overlapping serial jobs if one blocks/gets slow
        AtomicBoolean serialBusy = new AtomicBoolean(false);

        // Periodic control loop every 400 ms
        vertx.setPeriodic(400, id -> {
            if (!serialBusy.compareAndSet(false, true)) {
                return; // previous serial cycle still running
            }

            Callable<Void> blockingSerialTask = () -> {
                Status st = broker.getSystemController().getStatus();

                if (st == Status.UNCONNECTED) {
                    channelControllerManager.sendMessageToArduino(0);
                    //System.out.println("### UNCONNECTED ###");
                    return null;
                }

                if (broker.getSystemController().getIsManual() || st == Status.MANUAL) {
                    // Manual: operator command
                    channelControllerManager.sendMessageToArduino(broker.getSystemController().getValveValue());
                    
                    //System.out.println("### MANUAL ###");
                    return null;
                }

                // Automatic: commanded valve (0/50/100)
                int commanded = broker.getSystemController().getValveValue();
                channelControllerManager.sendMessageToArduino(commanded);
                
                //System.out.println("### AUTO ###");

                // Optional readback + validation
                String msg = channelControllerManager.receiveDataFromArduino();
                if (msg != null && !msg.isBlank()) {
                    broker.getSystemController().checkValveValue(msg, broker);
                }

                return null;
            };

            vertx.executeBlocking(blockingSerialTask, false, ar -> {
                serialBusy.set(false);
                if (ar.failed()) {
                    ar.cause().printStackTrace();
                }
            });
        });
    }
}
