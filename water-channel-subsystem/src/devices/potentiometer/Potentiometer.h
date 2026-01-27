#ifndef __POTENTIOMETER__
#define __POTENTIOMETER__

class Potentiometer {
public:
    Potentiometer();
    // virtual bool isPressed() = 0;
    // virtual bool isClicked() = 0;
    virtual void sync();
    long getLastSynchTime();

protected:
    virtual void updateSyncTime(long time);

private:
    long lastTimeSync;
};

#endif