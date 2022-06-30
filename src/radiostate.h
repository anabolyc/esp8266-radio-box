#pragma once

#include "radio.h"
#include "main.h"
#include <Ticker.h>
#include <RDSParser.h>

#define REFRESH_STATE_PERIOD_MS 1000

struct RadioCapabilities
{
  bool volume;
  bool bassBoost;
  bool rds;
};

class RadioState {
    public:
        RadioState(RadioCapabilities, Radio *);
        
        int volume;
        bool mono = false;
        bool power = true;
        bool bassBoost;
        RADIO_BAND band;
        RADIO_FREQ freq; 
        RadioCapabilities caps;

        Radio* radio;

        void setPower(bool);

    private:
        Ticker* _ticker;
        static RDSParser* rds;
};