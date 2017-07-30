#include "RadioScreen.h"

Adafruit_SSD1306 *display;

RadioScreen::RadioScreen(void) {
    
    display = new Adafruit_SSD1306(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

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

double RadioScreen::getVolume()
{
    return -5;
}
