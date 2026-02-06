#ifndef __USER_PANEL__
#define __USER_PANEL__

#include "HWPlatform.h"

/*
 * The WCS subsystem is equipped also with an LCD display reporting:
    the current valve opening level of the water channel.
    the current mode (AUTOMATIC or MANUAL), or UNCONNECTED (see later).
 */
class UserPanel {
public:
    // pHW è la piattaforma hardware da cui prendiamo il Button di reset
    UserPanel(HWPlatform* pHW);

    // inizializzazione logica (reset degli stati interni)
    void init();
    // metodi LCD
    void turnOnDisplay();
    void turnOffDisplay();

    void displayOpeningLevel(float val);
    //void displaySystemState();
    void displayAutomatic();
    void displayManualRemote();
    void displayManualLocal();
    void displayUnconnected();
    
    void prepareToSleep();
    void resumeFromSleeping();

    // Legge lo stato fisico del bottone e aggiorna le variabili interne.
    // Va chiamata periodicamente (es. ad ogni tick di un Task).
    void sync();

    // Ritorna true se il bottone è attualmente premuto
    bool isResetPressed() const;

    // Ritorna true SOLO quando il bottone passa da "non premuto" a "premuto"
    // (fronte di salita). Utile per non reagire mille volte mentre è tenuto premuto.
    bool isResetPressedEdge();
    void displayWaterLevel(int waterLevel);
private:
    HWPlatform* pHW;
    Button* pResetButton;
    LiquidCrystal_I2C* pLcd;
    

    bool resetPressed;      // stato attuale
    bool prevResetPressed;  // stato al tick precedente
};

#endif
