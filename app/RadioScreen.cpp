#include "RadioScreen.h"

Adafruit_SSD1306 *display;

RadioScreen::RadioScreen(void) 
{
    display = new Adafruit_SSD1306(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
}

void RadioScreen::init() 
{
	display->begin(SSD1306_SWITCHCAPVCC);
	display->display();

    display->clearDisplay();
	display->setTextSize(1);
	display->setTextColor(WHITE);
	display->setCursor(0,0);
	display->println("Sming Framework");
	display->setTextColor(BLACK, WHITE);
	display->setCursor(104, 7);
	display->println("v1.0"); 
    display->display();
}

int RadioScreen::setVolume(int value)
{
    return value;
}

int RadioScreen::getVolume()
{
    return 0;
}

bool RadioScreen::setMono(bool value)
{
    return value;
}

bool RadioScreen::getMono()
{
    return false;
}

RADIO_BAND RadioScreen::getBand() 
{
    return RADIO_BAND_NONE;
}

RADIO_BAND RadioScreen::setBand(RADIO_BAND band) 
{
    return band;
}

RADIO_FREQ RadioScreen::getFrequency() 
{
    return 0;
}

RADIO_FREQ RadioScreen::setFrequency(RADIO_FREQ freq) 
{
    return freq;
}