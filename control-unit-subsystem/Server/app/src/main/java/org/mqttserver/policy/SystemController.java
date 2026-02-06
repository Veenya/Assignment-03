package org.mqttserver.policy;

// import io.vertex.core.buffer.Buffer;
import org.mqttserver.presentation.Status;
import org.mqttserver.services.mqtt.Broker;

import java.util.Map;

public interface SystemController {

    //TODO
    /*
    Dato che CUS e' il controllore/capo dei COMANDI e dello STATO LOGICO, aggiungere
        desired/commanded J(deciso da CUS) -> isManual, status, valveCommand -> li abbiamo gia
        e measured/reported (riportati da arduino) -> valveActual, arduinoMode, lastSerialSeenAt

    aggiungere (feedback arduino)
        valveActual (valore reale riportato)
        isArduinoConnected / lastArduinoRxAt
        opzionale: arduinoModeReported (solo per diagnostica)
    
    */

    /*
    
    3) Policy CUS: aggiorno status/valveCommand quando cambia wl o mode

Due punti di trigger:

ogni volta che cambia wl (da MQTT o seriale)

e/o con un timer periodico (es. ogni 200ms)

In automatico:

calcolo status e quindi valveCommand (0/50/100) con la tua setValveValueAuto()

In manuale:

status = MANUAL, valveCommand = manualValveValue
    
    */
    void setWL(float wl);

    Status getStatus();
    float getWl();
    int getValveValue();

    void setValveValueFromDashboard(int valveValue);
    Map<Status, Integer> getStatusValveValue();

    void checkValveValue(String msg, Broker broker);

    void setIsManual(boolean isManual);
    boolean getIsManual(); 
    
    void updateConnectivity();
}
