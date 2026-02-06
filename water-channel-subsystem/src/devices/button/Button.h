#ifndef __BUTTON__
#define __BUTTON__

class Button {
public:
    Button();
    virtual bool isPressed() = 0;
    virtual bool isClickedAndReset() = 0;
    virtual void sync();
    long getLastSynchTime();

protected:
    virtual void updateSyncTime(long time);

private:
    long lastTimeSync;
};

#endif