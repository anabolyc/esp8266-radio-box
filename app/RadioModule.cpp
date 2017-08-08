#include "RadioModule.h"

RDA5807M *radio;

const RADIO_BAND FIX_BAND = RADIO_BAND_FM;
const RADIO_FREQ FIX_STATION = 9920;

RadioModule::RadioModule(void)
{
    radio = new RDA5807M();
}

void RadioModule::init() 
{
    radio->debugEnable(false);
    radio->init();
    radio->setBand(FIX_BAND);
    radio->setFrequency(FIX_STATION);
    radio->setVolume(0);
    radio->setMono(false);
}

int RadioModule::getVolume()
{
    return radio->getVolume();
}

bool RadioModule::setBassBoost(bool value)
{
    radio->setBassBoost(value);
    return value;
}

bool RadioModule::getBassBoost()
{
    return radio->getBassBoost();
}

int RadioModule::setVolume(int value)
{
    radio->setMute(value == 0);
    radio->setVolume(value);
    return value;
}

bool RadioModule::getMono()
{
    return radio->getMono();
}

bool RadioModule::setMono(bool value)
{
    radio->setMono(value);
    return value;
}

RADIO_BAND RadioModule::getBand() 
{
    return radio->getBand();
}

RADIO_BAND RadioModule::setBand(RADIO_BAND band) 
{
    radio->setBand(band);
    return band;
}

RADIO_FREQ RadioModule::getFrequency() 
{
    return radio->getFrequency();
}

RADIO_FREQ RadioModule::setFrequency(RADIO_FREQ freq) 
{
    radio->setFrequency(freq);
    return radio->getFrequency();
}

void RadioModule::seekUp() 
{
    radio->seekUp();
}

void RadioModule::seekDown() 
{
    radio->seekDown();
}

void RadioModule::getAudioInfo(AUDIO_INFO *info) {
    radio->getAudioInfo(info);
}

void RadioModule::getRadioInfo(RADIO_INFO *info) {
    radio->getRadioInfo(info);
}