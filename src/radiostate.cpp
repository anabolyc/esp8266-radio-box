#include "radiostate.h"
#include "logger.h"

RDSParser* RadioState::rds = new RDSParser();

RadioState::RadioState(RadioCapabilities caps, Radio * svc) {
    this->radio = svc;
    this->caps = caps;

    if (caps.rds) {
        
        radio->attachReceiveRDS([](uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4) {
            rds->processData(block1, block2, block3, block4);
        });

        rds->attachServicenNameCallback([](char *name)
        {
           logger->printf("attachServicenNameCallback: %s\n", name);
        });

        rds->attachTextCallback([](char *name)
        {
           logger->printf("attachTextCallback: %s\n", name);
        });
        
        rds->attachTimeCallback([](uint8_t hour, uint8_t minute)
        {
           logger->printf("attachTimeCallback: %d %d\n", hour, minute);
        });
    }

    _ticker = new Ticker();
    _ticker->attach_ms(REFRESH_STATE_PERIOD_MS, [&]() {
        // ??this->power = radio->getMute();
        this->volume = radio->getVolume();
        this->mono = radio->getMono();
        this->band = radio->getBand();
        this->freq = radio->getFrequency();
        this->bassBoost = radio->getBassBoost();
        #if (DEBUG > 1)
        {
            RADIO_INFO info;
            radio->getRadioInfo(&info);

            logger->print("VOL: ");
            logger->print(this->volume);
            logger->print("  FRQ: ");
            logger->print(this->freq);
            logger->print("  PWR: ");
            logger->print(this->power);

            logger->print(info.rds ? " RDS" : " ---");
            logger->print(info.tuned ? " TUNED" : " -----");
            logger->print(info.stereo ? " STEREO" : "  MONO ");
            logger->print("  RSSI: ");
            logger->print(info.rssi);
            logger->print("  S/N: ");
            logger->print(info.snr);
        }
        {
            AUDIO_INFO info;
            radio->getAudioInfo(&info);

            logger->print(info.bassBoost ? " BASS" : " ----");
            logger->print(info.mute ? " MUTE" : " ----");
            logger->print(info.softmute ? " SOFTMUTE" : " --------");
            logger->println();
        }
        #endif
    });
}

void RadioState::setPower(bool value) {
    if ((power) && (!value)) {
        radio->term();
        power = false;
    }

    if ((!power) && value) {
        radio->init();
        power = true;
    }
}