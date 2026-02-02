package org.mqttserver.presentation;

// Messages received by TMS (ESP)
// The sensor sends the WL (water level)

public class MessageFromSensor {
    private float WL;

    public MessageFromSensor(float WL) {
        this.WL = WL;
    }

    public float getWL() {
        return WL;
    }

    public void setWL(float WL) {
        this.WL = WL;
    }
}
