#ifndef RADIO_MODULE_H
#define RADIO_MODULE_H

#include <TEA5767.h>

class RadioModule : public TEA5767
{
    public:
        RadioModule();
        virtual void init();
};

#endif
