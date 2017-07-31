#ifndef I_RADIO_H
#define I_RADIO_H

class IRadio {
   public:
    IRadio() {};
    
    virtual void init();
    virtual int setVolume(int value);
    virtual double getVolume() = 0;
};

#endif