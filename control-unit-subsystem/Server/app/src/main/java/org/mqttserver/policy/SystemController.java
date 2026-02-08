package org.mqttserver.policy;

import org.mqttserver.presentation.ArduinoConnectionStatus;
import org.mqttserver.presentation.ESPConnectionStatus;
import org.mqttserver.presentation.SystemStatus;
import org.mqttserver.presentation.ValveStatus;

public interface SystemController {

    void setWL(float wl);
    void setValveValue(int valveValue);
    
    //void setValveCommand(int val);

    SystemStatus getSystemStatus();
    ValveStatus getValveStatus();
    ESPConnectionStatus getEspConnectionStatus();
    ArduinoConnectionStatus getArduinoConnectionStatus();

    float getWl();
    int getValveValue();
    //int getValveCommand();

    void setSystemStatus(SystemStatus systemStatus);
    void setValveStatus(ValveStatus valveStatus);
    void setEspConnectionStatus(ESPConnectionStatus espConnStatus);
    void setArduinoConnectionStatus(ArduinoConnectionStatus arduinoConnStatus);

    void setIsManualLocal();
    void setIsManualRemote();
    void setIsAutomatic();

    void updateConnectivity(); // aggiorna gli stati di connessione dei device
    void updatePolicy(); // Calcola status/valveCommand, da fare uando arriva wl

    boolean getIsManual();

    boolean getIsManualLocal();
    boolean getIsManualRemote();
    boolean getIsAutomatic();
    void resetLastArduinoConnection();
    void resetLastESPConnection();
}
