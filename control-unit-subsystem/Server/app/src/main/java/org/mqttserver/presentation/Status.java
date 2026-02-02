package org.mqttserver.presentation;

/**
 * System-level states.
 *
 * The dashboard must show: MANUAL, AUTOMATIC, UNCONNECTED, NOT_AVAILABLE (or NOT AVAILABLE).
 * In AUTOMATIC mode, the control policy can command the valve to:
 * - CLOSED (0%)
 * - OPEN_50 (50%) when WL > L1 for more than T1 (and WL < L2)
 * - OPEN_100 (100%) immediately when WL >= L2
 */
public enum Status {

    /**
     * Dashboard cannot reach the CUS (server down / unreachable).
     * This is a DBS-side state, but useful to model in shared domain if you want to display it consistently.
     */
    NOT_AVAILABLE,

    /**
     * CUS is not receiving data from TMS for more than T2.
     */
    UNCONNECTED,

    /**
     * System is in MANUAL mode: operator controls valve opening (potentiometer/dashboard).
     */
    MANUAL,

    /**
     * System is in AUTOMATIC mode: valve controlled by policy based on WL and time thresholds.
     * Sub-states below describe the commanded valve level while in automatic.
     */
    AUTO_CLOSED,     // 0% (default when WL <= L1)
    AUTO_OPEN_50,    // 50% (WL > L1 for > T1, and WL < L2)
    AUTO_OPEN_100,   // 100% (WL >= L2)

    /**
     * Input/state inconsistent (e.g., invalid sensor payload).
     */
    INVALID_STATUS;

    /** Convenience helpers (optional but practical). */
    public boolean isAutomatic() {
        return this == AUTO_CLOSED || this == AUTO_OPEN_50 || this == AUTO_OPEN_100;
    }

    public boolean isConnected() {
        return this != UNCONNECTED && this != NOT_AVAILABLE;
    }

    /** Commanded valve opening percentage for the status, if applicable. */
    public int valvePercent() {
        return switch (this) {
            case AUTO_CLOSED -> 0;
            case AUTO_OPEN_50 -> 50;
            case AUTO_OPEN_100 -> 100;
            default -> -1; // not applicable (manual/unconnected/not available/invalid)
        };
    }

    @Override
    public String toString() {
        return switch (this) {
            case NOT_AVAILABLE -> "NOT AVAILABLE";
            case UNCONNECTED -> "UNCONNECTED";
            case MANUAL -> "MANUAL";
            case AUTO_CLOSED -> "AUTOMATIC (0%)";
            case AUTO_OPEN_50 -> "AUTOMATIC (50%)";
            case AUTO_OPEN_100 -> "AUTOMATIC (100%)";
            case INVALID_STATUS -> "INVALID STATUS";
        };
    }
}
