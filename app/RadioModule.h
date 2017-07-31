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
        virtual double getVolume();
};

#endif