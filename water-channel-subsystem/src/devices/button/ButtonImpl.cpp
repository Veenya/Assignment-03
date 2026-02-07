#include "ButtonImpl.h"
#include "Arduino.h"

ButtonImpl::ButtonImpl(int pin) 
    : pin(pin), pressed(false), clicked(false), lastSyncTime(0) 
{
    pinMode(pin, INPUT);           // pull-down esterno → assumiamo tua resistenza
    sync();                        // inizializza stato iniziale
}

void ButtonImpl::sync() {
    // Chiamare regolarmente (es. ogni 5–20 ms nel loop principale)
    
    bool wasPressed = pressed;
    pressed = digitalRead(pin) == HIGH;
    
    lastSyncTime = millis();

    // clicked diventa true SOLO sul fronte di discesa (release)
    if (!pressed && wasPressed) {
        clicked = true;
    }
    // se è premuto → azzera subito il flag (comportamento standard)
    else if (pressed) {
        clicked = false;
    }
    // se è rilasciato da tempo → non tocchiamo clicked (aspettiamo che venga letto)
}

bool ButtonImpl::checkAndConsumeClick() {
    // restituisce true se c'è stato un "click" (premi → rilascia) 
    // dall'ultima chiamata a questo metodo (o dall'ultimo sync + consume)
    
    bool hadClick = clicked;
    clicked = false;           // consuma l'evento → resetta per il futuro
    return hadClick;
}

// Metodi di sola lettura (utili per debug o logica particolare)
bool ButtonImpl::isPressed() {
    return pressed;
}

bool ButtonImpl::hasPendingClick() {
    return clicked;
}

void ButtonImpl::debugPrint() {
    Serial.print(F("Button pin "));
    Serial.print(pin);
    Serial.print(F("  pressed: "));
    Serial.print(pressed ? "YES" : "no");
    Serial.print(F("  clicked(pending): "));
    Serial.println(clicked ? "YES" : "no");
}