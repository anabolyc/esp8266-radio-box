#pragma once

#define EEPROM_MAGIC_NUMBER 0xDF

#include "radio.h"

struct RadioState_t
{
    int volume;
    bool mono;
    bool bassBoost;
    RADIO_BAND band;
    RADIO_FREQ freq;
};

class SavedState
{
public:
    SavedState();

    void init(void);
    bool isValid();
    void load(RadioState_t *);
    void save(RadioState_t *);
    void erase();
};
