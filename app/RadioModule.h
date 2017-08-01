#ifndef RADIO_MODULE_H
#define RADIO_MODULE_H

#include "IRadio.h"

#include <radio.h>
#include <TEA5767.h>

class RadioModule : public IRadio
{
    public:
        RadioModule();

        virtual void init();
        virtual int setVolume(int value);
        virtual bool setMono(bool value);
        virtual RADIO_BAND setBand(RADIO_BAND band);
        virtual RADIO_FREQ setFrequency(RADIO_FREQ freq);
};

#endif
