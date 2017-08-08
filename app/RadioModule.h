#ifndef RADIO_MODULE_H
#define RADIO_MODULE_H

#include "IRadio.h"

#include <radio.h>
#include <TEA5767.h>
#include <RDA5807M.h>

class RadioModule : public IRadio
{
    public:
        RadioModule();

        virtual void init();
        virtual int getVolume();
        virtual int setVolume(int value);
        virtual bool getMono();
        virtual bool setMono(bool value);
        virtual bool setBassBoost(bool value);
        virtual bool getBassBoost();
        virtual RADIO_BAND getBand();
        virtual RADIO_BAND setBand(RADIO_BAND band);
        virtual RADIO_FREQ getFrequency();
        virtual RADIO_FREQ setFrequency(RADIO_FREQ freq);
        
        virtual void seekUp();
        virtual void seekDown();
        
        virtual void getAudioInfo(AUDIO_INFO *info);
        virtual void getRadioInfo(RADIO_INFO *info);
};

#endif
