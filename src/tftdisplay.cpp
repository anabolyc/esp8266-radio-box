#include "tftdisplay.h"

TftDisplay::TftDisplay(TFT_eSPI* _tft, RadioState* _state) {
    state = _state;
    tft = _tft;
}

void TftDisplay::init(void) {
    tft->init();
    tft->setRotation(TFT_ROTATION);
    tft->fillScreen(TFT_BLACK);   

    _ticker = new Ticker();
    _ticker->attach_ms(UPDATE_SCREEN_PERIOD_MS, [&]() {
        
        uint8_t smaller_font = 2, smaller_font_height = 16;
        uint8_t small_font = 4;
        uint8_t large_font = 7;

        RADIO_INFO radioinfo;
        AUDIO_INFO audioinfo;
        state->radio->getRadioInfo(&radioinfo);
        state->radio->getAudioInfo(&audioinfo);

        // tft->fillScreen(TFT_BLACK);
        
        if (state->power) {

            // logger->print(radioinfo.rds ? " RDS" : " ---");
            
            if (firstDraw) {
                tft->setTextSize(1);
                tft->setTextColor(TFT_WHITE);
                tft->setTextDatum(TC_DATUM);
                tft->drawString("Radio Box", tft->width() / 2, 0, small_font);
            }
            
            if ((last_band != state->band) || (last_freq != state->freq)) {

                String band = "", freq = "";
                uint8_t decimals = 0;
                switch (state->band) 
                {
                    case RADIO_BAND_FM:
                    case RADIO_BAND_FMTX:
                    case RADIO_BAND_FMWORLD:
                        band = "FM";
                        freq = "MHz";
                        decimals = 1;
                        break;
                    case RADIO_BAND_AM:
                        band = "AM";
                        freq = "KHz";
                        break;
                    case RADIO_BAND_KW:
                        band = "KW";
                        freq = "KHz";
                        break;
                    case RADIO_BAND_NONE:
                        break;
                }

                tft->setTextColor(TFT_DARKGREY);
                tft->setTextDatum(CL_DATUM);
                tft->drawString(band, 0, tft->height() / 2, small_font);
                tft->setTextDatum(CR_DATUM);
                tft->drawString(freq, tft->width(), tft->height() / 2, small_font);
            
                tft->setTextSize(1);
                tft->setTextDatum(MC_DATUM);
                tft->setTextColor(TFT_BLACK);
                tft->drawFloat(last_freq / 100., decimals, tft->width() / 2, tft->height() / 2, large_font);
                tft->setTextColor(radioinfo.tuned ? TFT_GREEN : TFT_YELLOW);
                tft->drawFloat(state->freq / 100., decimals, tft->width() / 2, tft->height() / 2, large_font);

                last_band = state->band;
                last_freq = state->freq;
            }

            tft->fillRect(0, tft->height() - smaller_font_height, tft->width(), smaller_font_height, TFT_BLACK);

            tft->setTextSize(1);
            tft->setTextColor(TFT_LIGHTGREY);
            tft->setTextDatum(BL_DATUM);
            tft->drawString(radioinfo.stereo ? "STEREO" : "MONO", 0, tft->height(), smaller_font);
            
            tft->setTextDatum(BR_DATUM);
            tft->setTextColor(audioinfo.bassBoost ? TFT_LIGHTGREY : TFT_DARKGREY);
            tft->drawString("BASS BOOST", tft->width(), tft->height(), smaller_font);

            uint8_t rect_x = 5;
            uint8_t rect_y = 2;
            uint8_t start_x = tft->width() / 2 - 10 * rect_x;
            uint8_t start_y = tft->height() / 4 * 3;
            uint16_t rect_color = TFT_CYAN;

            if (last_volume != state->volume) {
                for (uint8_t i = 0; i < 10; i++) {
                    uint8_t x = start_x + i * rect_x * 2;

                    tft->fillRect(x, start_y - rect_y * (i + 1), rect_x, rect_y * (i + 1), TFT_BLACK);
                    if (i < state->volume) {
                        tft->fillRect(x, start_y - rect_y * (i + 1), rect_x, rect_y * (i + 1), rect_color);
                    } else {
                        tft->drawRect(x, start_y - rect_y * (i + 1), rect_x, rect_y * (i + 1), rect_color);
                    }
                }
                
                last_volume = state->volume;
            }

            if (firstDraw) {
                start_x = tft->width() / 2 - 10 * rect_x - 15;
                tft->fillRect(start_x - 10, start_y - rect_y * 8, 10, rect_y * 6, rect_color);
                tft->fillTriangle(start_x, start_y, start_x, start_y - rect_y * 10, start_x - 10, start_y - rect_y * 10 / 2, rect_color);
            }


            // rssi is in the range of 15 to 25
            uint8_t rssi = (radioinfo.rssi < 15 ? 0 : radioinfo.rssi - 15) / 2; // 0 to 5 now

            if (last_rssi != rssi) {
                rect_y = 4;
                start_x = tft->width() / 2 - 5 * rect_x;
                start_y = tft->height() / 4;
                for (uint8_t i = 0; i < 5; i++) {
                    uint8_t x = start_x + i * rect_x * 2;
                    tft->fillRect(x, start_y - rect_y * (i + 1), rect_x, rect_y * (i + 1), TFT_BLACK);
                    if (i < rssi) {
                        tft->fillRect(x, start_y - rect_y * (i + 1), rect_x, rect_y * (i + 1), rect_color);
                    } else {
                        tft->drawRect(x, start_y - rect_y * (i + 1), rect_x, rect_y * (i + 1), rect_color);
                    }
                }

                last_rssi = rssi;
            }

            if (firstDraw) {
                start_x = tft->width() / 2 - 5 * rect_x - 10;
                tft->drawLine(start_x, start_y, start_x, start_y - rect_y * 5, rect_color);
                tft->drawLine(start_x, start_y - rect_y * 5 / 2, start_x - rect_y * 5 / 2, start_y - rect_y * 5, rect_color);
                tft->drawLine(start_x, start_y - rect_y * 5 / 2, start_x + rect_y * 5 / 2, start_y - rect_y * 5, rect_color);
            }

            firstDraw = false;
        } 
    });
}