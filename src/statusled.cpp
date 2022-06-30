#include "statusled.h"
#include "logger.h"

uint32_t led_time_on = 0;
uint32_t led_time_off = 0;
uint32_t led_color = 0;

Ticker * _blinker = new Ticker();
CRGB leds[PIN_STATUS_LED_COUNT];


static uint32_t StatusLedBlinkTimings[LED_STATUSES_COUNT][LED_STATUSES_STATES_COUNT + 1] = {
    // on, off - in milliseconds, color (R,G,B)
    {1024, 16, 0x000080},   // NOT_READY
    {16, 1024, 0x808000},   // READY
    {256, 256, 0x800080},   // CLIENT_CONNECTED
    // {256, 0, 0x008000},  // CLIENT_RX
    // {256, 0, 0x000080},  // CLIENT_TX
    // {128, 128, 0x800000} // ERROR
    {0, 1024, 0x00}         // UNDEFINED
};

StatusLed::StatusLed() {
    //_blinker = new Ticker();
    FastLED.addLeds<WS2812B, PIN_LED, GRB>(leds, PIN_STATUS_LED_COUNT);
    setState(NOT_READY);
}

void StatusLed::setState(ApplicationState state) 
{
    if (state == _state)
        return;

    logger->printf("Status changed to %d\n", state);
    _state = state;

    if (_blinker->active())
        _blinker->detach();
    
    led_time_on = StatusLedBlinkTimings[(int)state][0];
    led_time_off = StatusLedBlinkTimings[(int)state][1];
    led_color = StatusLedBlinkTimings[(int)state][2];

    if (led_time_off == 0) {
        // no need to switch off
        for (uint8_t i = 0; i < PIN_STATUS_LED_COUNT; i++)
            leds[0] = led_color;
        FastLED.show();
    } else if (led_time_on == 0) {
        // no need to switch on
        for (uint8_t i = 0; i < PIN_STATUS_LED_COUNT; i++)
            leds[0] = 0;
        FastLED.show();
    } else {
        // trigger blinking
        changeStatusLedStateOn();
    }
}

void StatusLed::changeStatusLedStateOn()
{
    //logger->printf("Led is on: %d\n", led_color);
    for (uint8_t i = 0; i < PIN_STATUS_LED_COUNT; i++)
        leds[0] = led_color;
    FastLED.show();
    _blinker->once_ms(led_time_on, changeStatusLedStateOff);
}

void StatusLed::changeStatusLedStateOff()
{
    //logger->printf("Led is off: %d\n", 0x0);
    for (uint8_t i = 0; i < PIN_STATUS_LED_COUNT; i++)
        leds[0] = 0x0;
    FastLED.show();
    _blinker->once_ms(led_time_off, changeStatusLedStateOn);
}
