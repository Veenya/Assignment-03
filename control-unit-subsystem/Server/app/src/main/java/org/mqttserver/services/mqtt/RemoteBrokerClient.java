package org.mqttserver.services.mqtt;

import org.mqttserver.policy.SystemController;

public interface RemoteBrokerClient {

    /**
     * Avvia la connessione al broker MQTT remoto e si sottoscrive ai topic necessari.
     */
    void start();

    /**
     * Disconnette il client e chiude le risorse (Vertx).
     */
    void stop();

    /**
     * Espone il controller di sistema aggiornato dai messaggi ricevuti.
     */
    SystemController getSystemController();
}
