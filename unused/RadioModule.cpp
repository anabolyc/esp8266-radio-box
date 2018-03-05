#include "RadioModule.h"

const RADIO_BAND FIX_BAND = RADIO_BAND_FM;
const RADIO_FREQ FIX_STATION = 9920;

RadioModule::RadioModule(void)
{
    //radio = new RDA5807M();
    //radio = new TEA5767();
}

void RadioModule::init()
{
    radio->debugEnable(false);
    TEA5767::init();
    delay(100);

    radio->setBand(FIX_BAND);
    radio->setFrequency(FIX_STATION);
    radio->setVolume(1);
    radio->setMono(false);
}
