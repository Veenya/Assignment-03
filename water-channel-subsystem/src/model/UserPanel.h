// water-channel-subsystem/src/model/UserPanel.h
#ifndef __USER_PANEL__
#define __USER_PANEL__

#include "HWPlatform.h"

class UserPanel {
public:
    // pHW è la piattaforma hardware da cui prendiamo il Button di reset e l’LCD
    UserPanel(HWPlatform* pHW);

    // inizializzazione logica (reset degli stati interni)
    void init();

    // metodi LCD
    void turnOnDisplay();
    void turnOffDisplay();

    void displayOpeningLevel(float val);
    void displayAutomatic();
    void displayManual();
    void displayUnconnected();

    void prepareToSleep();
    void resumeFromSleeping();

    // aggiorna stato pulsante
    void sync();

    // stato attuale (livello)
    bool isResetPressed() const;

    // true solo nel tick in cui viene premuto (fronte di salita)
    bool isResetPressedEdge() const;

private:
    HWPlatform* pHW;
    Button* pResetButton;
    LiquidCrystal_I2C* pLcd;

    bool resetPressed;
    bool prevResetPressed;
};

#endif
