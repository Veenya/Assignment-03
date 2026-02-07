#ifndef __BUTTON_IMPL__
#define __BUTTON_IMPL__

#include "Button.h"

class ButtonImpl : public Button {
public:
    ButtonImpl(int pin);
    bool isPressed() override;
    void sync() override;
    bool checkAndConsumeClick();
    bool hasPendingClick();
    void debugPrint();

private:
    int pin;
    bool pressed;
    bool clicked;
    unsigned long lastSyncTime;
};

#endif