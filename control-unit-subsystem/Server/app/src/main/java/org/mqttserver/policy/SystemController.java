package org.mqttserver.policy;

// import io.vertex.core.buffer.Buffer;
import org.mqttserver.presentation.Status;
import org.mqttserver.services.mqtt.Broker;

import java.util.Map;

public interface SystemController {

    //TODO


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
    //void setValveCommand(int val);

    Status getStatus();
    float getWl();
    int getValveValue();
    //int getValveCommand();
    

    void setValveValueFromDashboard(int valveValue);
    Map<Status, Integer> getStatusValveValue();

    void checkValveValue(String msg, Broker broker);

    void setIsManual(boolean isManual);
    boolean getIsManual(); 
    
    void updateConnectivity();
    void updatePolicy(); // Calcola status/valveCommand, da fare uando arriva wl
}
