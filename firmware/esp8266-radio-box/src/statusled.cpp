#include "statusled.h"
#include "logger.h"

uint32_t led_time_on = 0;
uint32_t led_time_off = 0;
uint32_t led_color = 0;

Ticker *_blinker = new Ticker();
#if (PIN_LED_TYPE == 0)
CRGB leds[PIN_STATUS_LED_COUNT];
#endif

static uint32_t StatusLedBlinkTimings[LED_STATUSES_COUNT][LED_STATUSES_STATES_COUNT + 1] = {
    // on, off - in milliseconds, color (R,G,B)
    {1024, 16, 0x000080}, // NOT_READY
    {16, 1024, 0x808000}, // READY
    {256, 256, 0x800080}, // CLIENT_CONNECTED
    {0, 1024, 0x00}       // UNDEFINED
};

StatusLed::StatusLed()
{
//_blinker = new Ticker();
#if (PIN_LED_TYPE == 0)
    FastLED.addLeds<WS2812B, PIN_LED, GRB>(leds, PIN_STATUS_LED_COUNT);
#else
    pinMode(PIN_LED, OUTPUT);
#endif
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

    if (led_time_off == 0)
    {
        // no need to switch off
#if (PIN_LED_TYPE == 0)
        for (uint8_t i = 0; i < PIN_STATUS_LED_COUNT; i++)
            leds[i] = led_color;
        FastLED.show();
#endif
#if (PIN_LED_TYPE == 1)
        digitalWrite(PIN_LED, HIGH);
#endif
#if (PIN_LED_TYPE == 2)
        digitalWrite(PIN_LED, LOW);
#endif
    }
    else if (led_time_on == 0)
    {
        // no need to switch on
#if (PIN_LED_TYPE == 0)
        for (uint8_t i = 0; i < PIN_STATUS_LED_COUNT; i++)
            leds[i] = 0;
        FastLED.show();
#endif
#if (PIN_LED_TYPE == 1)
        digitalWrite(PIN_LED, LOW);
#endif
#if (PIN_LED_TYPE == 2)
        digitalWrite(PIN_LED, HIGH);
#endif
    }
    else
    {
        // trigger blinking
        changeStatusLedStateOn();
    }
}

void StatusLed::changeStatusLedStateOn()
{
#if (PIN_LED_TYPE == 0)
    // logger->printf("Led is on: %d\n", led_color);
    for (uint8_t i = 0; i < PIN_STATUS_LED_COUNT; i++)
        leds[i] = led_color;
    FastLED.show();
    _blinker->once_ms(led_time_on, changeStatusLedStateOff);
#endif
#if (PIN_LED_TYPE == 1)
    digitalWrite(PIN_LED, HIGH);
#endif
#if (PIN_LED_TYPE == 2)
    digitalWrite(PIN_LED, LOW);
#endif
}

void StatusLed::changeStatusLedStateOff()
{
// logger->printf("Led is off: %d\n", 0x0);
#if (PIN_LED_TYPE == 0)
    for (uint8_t i = 0; i < PIN_STATUS_LED_COUNT; i++)
        leds[i] = 0x0;
    FastLED.show();
    _blinker->once_ms(led_time_off, changeStatusLedStateOn);
#endif
#if (PIN_LED_TYPE == 1)
    digitalWrite(PIN_LED, LOW);
#endif
#if (PIN_LED_TYPE == 2)
    digitalWrite(PIN_LED, HIGH);
#endif
}
