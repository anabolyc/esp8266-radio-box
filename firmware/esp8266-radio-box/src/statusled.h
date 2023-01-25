#pragma once

#include <Ticker.h>
#if (PIN_LED_TYPE==0)
#include <FastLED.h>
#else
#include <Arduino.h>
#endif

#define LED_STATUSES_COUNT 4
#define LED_STATUSES_STATES_COUNT 2
#define PIN_STATUS_LED_COUNT 1

enum ApplicationState {
    NOT_READY,          // 0
    READY,              // 1
    CLIENT_CONNECTED,   // 2
    UNDEFINED
};

class StatusLed {
    public:
        StatusLed();

        void setState(ApplicationState);
    private:
        ApplicationState _state = UNDEFINED;

        static void changeStatusLedStateOff(void);
        static void changeStatusLedStateOn(void);

};