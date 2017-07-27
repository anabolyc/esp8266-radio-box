#ifndef RADIO_SCREEN_H
#define RADIO_SCREEN_H

#include "IRadio.h"

class RadioScreen : public IRadio
{
    public:
        RadioScreen();
        virtual double getVolume();
};

#endif