#ifndef __POTENTIOMETER__
#define __POTENTIOMETER__

class Potentiometer {
public:
    Potentiometer();
    virtual ~Potentiometer();

    // “tick” opzionale: la base non fa nulla
    virtual void sync();

    long getLastSynchTime() const;

protected:
    void updateSyncTime(long time);

    long lastTimeSync;
};

#endif
