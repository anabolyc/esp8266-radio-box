#ifndef I_RADIO_H
#define I_RADIO_H

#include "radio_structs.h"

class IRadio {
   public:
    IRadio() {};
    
    virtual void init();
    
    virtual bool getPower();
    virtual void setPower(bool value);

    virtual uint8_t getVolume();
    virtual void setVolume(uint8_t value);

    virtual bool getMono();
    virtual void setMono(bool value);

    virtual bool getBassBoost();
    virtual void setBassBoost(bool value);

    virtual RADIO_BAND getBand();
    virtual void setBand(RADIO_BAND band);

    virtual RADIO_FREQ getFrequency();
    virtual void setFrequency(RADIO_FREQ freq);

    virtual RADIO_CAP getCapabilities();
    
    virtual void seekUp();
    virtual void seekDown();

    virtual void getAudioInfo(AUDIO_INFO *info);
    virtual void getRadioInfo(RADIO_INFO *info);

    //virtual void attachRdsCallback(receiveRDSFunction callback);
};

#endif