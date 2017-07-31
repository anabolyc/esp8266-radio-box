#ifndef RADIO_SCREEN_H
#define RADIO_SCREEN_H
#include <Arduino.h>

const int8_t OLED_MOSI  =  0; // D3
const int8_t OLED_CLK   = 16; // D0
const int8_t OLED_DC    = 12; // D6
const int8_t OLED_CS    = 13; // D7
const int8_t OLED_RESET = 14; // D5

#include "IRadio.h"

#include <Libraries/Adafruit_SSD1306/Adafruit_SSD1306.h>

class RadioScreen : public IRadio
{
    public:
        RadioScreen();

        virtual void init();
        virtual int setVolume(int value);
        virtual double getVolume();
};

#endif