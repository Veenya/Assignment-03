package org.mqttserver.policy;
import org.mqttserver.presentation.ArduinoConnectionStatus;
import org.mqttserver.presentation.ESPConnectionStatus;
import org.mqttserver.presentation.SystemStatus;
import org.mqttserver.presentation.ValveStatus;


import java.util.EnumMap;
import java.util.Map;

public class SystemControllerImpl implements SystemController {

    // Spec thresholds
    private final float L1 = 8f;
    private final float L2 = 16f;

    // Times (milliseconds)
    private final long T1_MS = 10_000; // 10 seconds above L1 before opening 50%
    private final long T2_MS = 15_000; // 15 seconds without data => UNCONNECTED

    // State
    private SystemStatus systemStatus = SystemStatus.MANUAL_LOCAL; // starting mode is AUTOMATIC
    private ArduinoConnectionStatus arduinoConnectionStatus = ArduinoConnectionStatus.KO;
    private ESPConnectionStatus espConnectionStatus = ESPConnectionStatus.KO;
    private ValveStatus valveStatus = ValveStatus.AUTO_CLOSED;

    private float wl = 0f;

    // Manual valve value (0..100)
    private int manualValveValue = 0;

    // For timing
    private long lastESPConnection = 0L;     // last received WL timestamp
    private long lastArduinoConnection = 0L; // last message from arduino timestamp
    private Long aboveL1SinceMs = null;      // when WL first went above L1 (used for T1)

    // Map status -> valve percent (automatic states only)
    private final Map<ValveStatus, Integer> statusValveValue = new EnumMap<>(ValveStatus.class);

    public SystemControllerImpl() {
        statusValveValue.put(ValveStatus.AUTO_CLOSED, 0);
        statusValveValue.put(ValveStatus.AUTO_OPEN_50, 50);
        statusValveValue.put(ValveStatus.AUTO_OPEN_100, 100);
    }

    /**
     * Called whenever a new water level sample is received from TMS.
     */
    @Override
    public void setWL(float wl) {
        this.wl = wl;
        updatePolicy();
    }

    @Override
    public void resetLastESPConnection() {
        System.out.println("Dentro resetLastESPConnection, updating ESP TMS");
        this.lastESPConnection = System.currentTimeMillis();
    }

    @Override
    public void resetLastArduinoConnection() {
        System.out.println("Dentro resetLastArduinoConnection, updating Arduino TMS");
        this.lastArduinoConnection = System.currentTimeMillis();
    }

    public void updateConnectivity() {
        long now = System.currentTimeMillis();
        // System.out.println("lastESPConnection " + lastESPConnection);
        // System.out.println("lastArduinoConnection " + lastArduinoConnection);
        // ESP
        if (lastESPConnection == 0L) {
            espConnectionStatus = ESPConnectionStatus.KO; // mai ricevuto
        } else if (now - lastESPConnection > T2_MS) {
            espConnectionStatus = ESPConnectionStatus.KO;
        } else {
            espConnectionStatus = ESPConnectionStatus.CONNECTED;
        }

        // Arduino
        if (lastArduinoConnection == 0L) {
            arduinoConnectionStatus = ArduinoConnectionStatus.KO; // mai ricevuto
        } else if (now - lastArduinoConnection > T2_MS) {
            arduinoConnectionStatus = ArduinoConnectionStatus.KO;

        } else {
            arduinoConnectionStatus = ArduinoConnectionStatus.CONNECTED;
        }

        // if (arduinoConnectionStatus == ArduinoConnectionStatus.KO
        //         && espConnectionStatus == ESPConnectionStatus.KO) {
        //     systemStatus = SystemStatus.MANUAL_LOCAL; // oppure un FALLBACK dedicato
        // } else if (arduinoConnectionStatus == ArduinoConnectionStatus.KO) {
        //     systemStatus = SystemStatus.MANUAL_LOCAL;
        // } else if (espConnectionStatus == ESPConnectionStatus.KO) {
        //     systemStatus = SystemStatus.MANUAL_REMOTE;
        // }
    }


    /**
     * Policy update that accounts for:
     * - MANUAL vs AUTOMATIC
     * - UNCONNECTED if no WL data for > T2
     * - AUTO valve states based on WL thresholds and T1 timing
     *
     * Call periodically (e.g., every 200ms/500ms) AND/OR after setWL().
     */
    @Override
    public void updatePolicy() {
        long now = System.currentTimeMillis();

        if (systemStatus == SystemStatus.AUTOMATIC) {
            // AUTO policy
            if (wl < L1) {
                // WL <= L1 => closed and reset timer
                valveStatus = ValveStatus.AUTO_CLOSED;
                aboveL1SinceMs = null;
            } else if (wl > L1 && wl < L2) {
                if (aboveL1SinceMs == null) {
                    aboveL1SinceMs = now; // start timing above L1
                }
                if (now - aboveL1SinceMs >= T1_MS) {
                    valveStatus = ValveStatus.AUTO_OPEN_50;
                }

            } else if (wl > L2) {
                valveStatus = ValveStatus.AUTO_OPEN_100;
            } 
        }
        

    }

    @Override
    public SystemStatus getSystemStatus() {
        return systemStatus;
    }

    
    @Override
    public ArduinoConnectionStatus getArduinoConnectionStatus() {
        return arduinoConnectionStatus;
    }

    
    @Override
    public ESPConnectionStatus getEspConnectionStatus() {
        return espConnectionStatus;
    }

    
    @Override
    public ValveStatus getValveStatus() {
        return valveStatus;
    }

    @Override
    public void setSystemStatus(SystemStatus systemStatus) {
        this.systemStatus = systemStatus;
    }

    
    @Override
    public void setArduinoConnectionStatus(ArduinoConnectionStatus arduinoConnectionStatus) {
        this.arduinoConnectionStatus = arduinoConnectionStatus;
    }

    
    @Override
    public void setEspConnectionStatus(ESPConnectionStatus espConnectionStatus) {
        this.espConnectionStatus = espConnectionStatus;
    }

    
    @Override
    public void setValveStatus(ValveStatus valveStatus) {
        this.valveStatus = valveStatus;
    }

    @Override
    public float getWl() {
        return wl;
    }

    /**
     * In AUTOMATIC, valve value is derived from status (0/50/100).
     * In MANUAL, it is the last manual value set by dashboard/operator.
     */
    @Override
    public int getValveValue() {
        if (getIsManual()) return manualValveValue;
        return statusValveValue.getOrDefault(valveStatus, 0);
    }

    @Override
    public void setValveValue(int valveValue) {
        // In spec, this is only meaningful in MANUAL mode
        this.manualValveValue = valveValue;
    }

    @Override
    public void setIsManualLocal() {
        this.systemStatus = SystemStatus.MANUAL_LOCAL;
        // Quando cambi mode riallinea la valvola
        updatePolicy();
    }

    @Override
    public void setIsManualRemote() {
        this.systemStatus = SystemStatus.MANUAL_REMOTE;
        // Quando cambi mode riallinea la valvola
        updatePolicy();
    }

    public void setIsAutomatic() {
        this.systemStatus = SystemStatus.AUTOMATIC;
    }

    @Override
    public boolean getIsAutomatic() {
        return this.systemStatus == SystemStatus.AUTOMATIC;
    }

    @Override
    public boolean getIsManual() {
        return this.systemStatus == SystemStatus.MANUAL_LOCAL || this.systemStatus == SystemStatus.MANUAL_REMOTE;
    }

    @Override
    public boolean getIsManualLocal() {
        return this.systemStatus == SystemStatus.MANUAL_LOCAL;
    }

    @Override
    public boolean getIsManualRemote() {
        return this.systemStatus == SystemStatus.MANUAL_REMOTE;
    }
}
