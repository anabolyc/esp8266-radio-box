#ifndef I_RADIO_H
#define I_RADIO_H

#include <radio.h>

class IRadio {
   public:
    IRadio() {};
    
    virtual void init();
    virtual int setVolume(int value);
    virtual bool setMono(bool value);
    virtual RADIO_BAND setBand(RADIO_BAND band);
    virtual RADIO_FREQ setFrequency(RADIO_FREQ freq);
};

#endif