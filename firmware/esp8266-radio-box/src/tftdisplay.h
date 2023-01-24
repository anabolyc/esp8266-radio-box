#include <TFT_eSPI.h>
#include <Ticker.h>

#include "radiostate.h" 

#define UPDATE_SCREEN_PERIOD_MS 500

class TftDisplay {
    public:
        TftDisplay(TFT_eSPI*, RadioState*);
        void init(void);    
    private:
        bool firstDraw = true;
        RADIO_BAND last_band = RADIO_BAND_NONE;
        RADIO_FREQ last_freq = 0;
        int last_volume = -1;
        int last_rssi = -1;

        TFT_eSPI* tft;
        Ticker* _ticker;
        RadioState* state;
        
};