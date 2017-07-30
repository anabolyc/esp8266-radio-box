#include "RadioModule.h"

TEA5767 *radio;

const RADIO_BAND FIX_BAND = RADIO_BAND_FM;
const RADIO_FREQ FIX_STATION = 10230;

RadioModule::RadioModule(void)
{
    radio = new TEA5767();
    radio->debugEnable(true);
    radio->init();

    /*
    radio->setBandFrequency(FIX_BAND, FIX_STATION);
    radio->setVolume(1);
    radio->setMono(false);
    
    */
}

double RadioModule::getVolume()
{
    return 5;
}