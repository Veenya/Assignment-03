package org.mqttserver.policy;

import io.vertx.core.buffer.Buffer;
import org.mqttserver.presentation.Status;
import org.mqttserver.services.mqtt.Broker;
import org.mqttserver.presentation.JSONUtils;
import org.mqttserver.presentation.MessageToArduino;
import org.mqttserver.presentation.MessageFromArduino;

import java.util.EnumMap;
import java.util.HashMap;
import java.util.Map;

//TODO: UNCONNECTED non può funzionare se non chiami updateConnectivity()
// Devi chiamarlo periodicamente nel CUS, per esempio con un timer (Vert.x setPeriodic) 
// oppure ogni volta che gestisci qualche evento.
// TODO: NOT_AVAILABLE è una cosa DBS-side
// Non ha senso calcolarla dentro CUS: 
// è la dashboard che decide “NOT_AVAILABLE” quando non riesce a fare HTTP verso CUS.

/*
È la parte “logica” del tuo progetto: tiene in memoria vari stati (es. manuale/automatico) e valori (WL).
Qui dentro aggiorni WL quando arriva un messaggio dal sensore.
 */

public class SystemControllerImpl implements SystemController {

    // Spec thresholds
    private final float L1 = 5f;
    private final float L2 = 20f;

    // Times (milliseconds)
    private final long T1_MS = 10_000; // 10 seconds above L1 before opening 50%
    private final long T2_MS = 15_000; // 15 seconds without data => UNCONNECTED

    // State
    private Status status = Status.AUTO_CLOSED; // starting mode is AUTOMATIC
    private float wl = 0f;

    // Manual mode flag
    private boolean isManual = false;

    // Manual valve value (0..100)
    private int manualValveValue = 0;

    // For timing
    private long lastTmsSampleAtMs = 0L;     // last received WL timestamp
    private Long aboveL1SinceMs = null;      // when WL first went above L1 (used for T1)

    // Map status -> valve percent (automatic states only)
    private final Map<Status, Integer> statusValveValue = new EnumMap<>(Status.class);

    public SystemControllerImpl() {
        statusValveValue.put(Status.AUTO_CLOSED, 0);
        statusValveValue.put(Status.AUTO_OPEN_50, 50);
        statusValveValue.put(Status.AUTO_OPEN_100, 100);
    }

    /**
     * Called whenever a new water level sample is received from TMS.
     */
    @Override
    public void setWL(float wl) {
        this.wl = wl;
        this.lastTmsSampleAtMs = System.currentTimeMillis();
        updatePolicy();
    }

    /**
     * TODO
     * Should be called periodically (by a timer in CUS) to update UNCONNECTED state.
     * If you don't call this anywhere, UNCONNECTED will never trigger.
     */
    public void updateConnectivity() {
        if (isManual) {
            // manual mode still depends on connectivity, but specification says UNCONNECTED is based on TMS data
            // keep manual unless no data for T2 -> then UNCONNECTED
        }

        long now = System.currentTimeMillis();
        if (lastTmsSampleAtMs == 0L) {
            // never received data
            status = Status.UNCONNECTED;
            return;
        }

        if (now - lastTmsSampleAtMs > T2_MS) {
            status = Status.UNCONNECTED;
        } else {
            // if we were unconnected, restore to a reasonable state
            if (status == Status.UNCONNECTED) {
                status = isManual ? Status.MANUAL : Status.AUTO_CLOSED;
            }
        }
    }


    public synchronized void updateConnectivityOnly() {
        long now = System.currentTimeMillis();
        if (lastTmsSampleAtMs == 0L || (now - lastTmsSampleAtMs) > T2_MS) {
            status = Status.UNCONNECTED;
        } else if (status == Status.UNCONNECTED) {
            status = isManual ? Status.MANUAL : Status.AUTO_CLOSED;
        }
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

        // Connectivity (UNCONNECTED if no samples for T2)
        if (lastTmsSampleAtMs == 0L || (now - lastTmsSampleAtMs) > T2_MS) {
            status = Status.UNCONNECTED;
            return;
        }

        // Manual mode overrides auto substates
        if (isManual) {
            status = Status.MANUAL;
            return;
        }

        // Validate WL
        if (wl < 0f) {
            status = Status.INVALID_STATUS;
            return;
        }

        // AUTO policy
        if (wl > L1) {
            if (aboveL1SinceMs == null) {
                aboveL1SinceMs = now; // start timing above L1
            }
            if (now - aboveL1SinceMs >= T1_MS) {
                status = Status.AUTO_OPEN_50;
            } else {
                status = Status.AUTO_CLOSED; // not yet above l1 for t1
            }
        } else {
            // WL <= L1 => closed and reset timer
            status = Status.AUTO_CLOSED;
            aboveL1SinceMs = null;
        }

    }

    @Override
    public Status getStatus() {
        return status;
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
        if (isManual) return manualValveValue;
        return statusValveValue.getOrDefault(status, 0);
    }

    @Override
    public void setValveValueFromDashboard(int valveValue) {
        // In spec, this is only meaningful in MANUAL mode
        this.manualValveValue = clampPercent(valveValue);
    }




    @Override
    public Map<Status, Integer> getStatusValveValue() {
        return statusValveValue;
    }

    @Override
    public void checkValveValue(String msg, Broker broker) {
        // This verifies the valve value reported by Arduino is consistent with the commanded valve value.
        try {
            Integer reported = JSONUtils.jsonToObject(msg, MessageFromArduino.class).getValveValue();
            int expected = broker.getSystemController().getValveValue();

            if (reported != null && reported == expected) {
                // ok: you can store it if you want to represent actual valve position
                // (in this simplified model, we just accept it)
            } else {
                System.err.println("SERVER: Valve value incorrect. Expected=" + expected + " reported=" + reported);
            }
        } catch (Exception ex) {
            System.err.println("SERVER: waiting for valid data from Arduino...");
        }
    }

    @Override
    public void setIsManual(boolean isManual) {
        this.isManual = isManual;
        // Quando cambi mode riallinea la valvola
        updatePolicy();
    }

    @Override
    public boolean getIsManual() {
        return isManual;
    }

    private int clampPercent(int v) {
        if (v < 0) return 0;
        if (v > 100) return 100;
        return v;
    }
}
