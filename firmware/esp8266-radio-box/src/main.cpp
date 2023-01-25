#include <Arduino.h>
#include <WiFiManager.h>
#include <TFT_eSPI.h>
#include <Ticker.h>

WiFiManager wifiManager;

#if defined(PIN_LED)
#include "statusled.h"
#define ENABLE_LED
StatusLed *statusLed = new StatusLed();
#endif

#include "webserver.h"
#include "tftdisplay.h"
#include "logger.h"
#include "main.h"

static Radio *radio = new Radio();
static RadioState *state = new RadioState(RadioCaps, radio);
TFT_eSPI *tft = new TFT_eSPI();
TftDisplay *tftdisplay = new TftDisplay(tft, state);
WebServer *server = new WebServer(state);

#if defined(PIN_BTN_L) && defined(PIN_BTN_M) && defined(PIN_BTN_R)
#include <OneButton.h>
#define ENABLE_BUTTONS
OneButton *btn_l = new OneButton(PIN_BTN_L, false, false);
OneButton *btn_m = new OneButton(PIN_BTN_M, false, false);
OneButton *btn_r = new OneButton(PIN_BTN_R, false, false);
#endif

void startWifi()
{
#ifdef ESP8266
	uint32_t chipID = ESP.getChipId();
#endif
#ifdef ESP32
	uint64_t macAddress = ESP.getEfuseMac();
	uint64_t macAddressTrunc = macAddress << 40;
	uint32_t chipID = macAddressTrunc >> 40;
#endif

	String ssid = "ESP-" + String(chipID);
	logger->printf("Setting hostname to %s\n", ssid.c_str());
	WiFi.hostname(ssid);

	wifiManager.setConfigPortalBlocking(false);
    wifiManager.setConfigPortalTimeout(60);
	// wifiTicker->attach(1, [&](){
	// 	logger->println("tick");
		if (!wifiManager.autoConnect())
		{
			logger->println("Failed to connect and hit timeout");
			#ifndef NO_WIFI_WAIT
			ESP.restart();
			#endif
		}
		else
		{
			logger->printf("Connected: ");
			logger->println(WiFi.localIP());
			server->init();
			#ifdef ENABLE_LED
			statusLed->setState(ApplicationState::READY);
			#endif
			// wifiTicker->detach();
		}
	// });
}

void setup()
{
	logger->begin(SERIAL_BAUD);

	Wire.begin(PIN_SDA, PIN_SCL);
	radio->init();
	radio->debugEnable(DEBUG > 0);
	delay(100);

	state->init();	
	logger->println("Radio services started");

	tftdisplay->init();

#ifdef ENABLE_BUTTONS
	pinMode(PIN_BTN_L, INPUT);
	pinMode(PIN_BTN_M, INPUT);
	pinMode(PIN_BTN_R, INPUT);

	btn_l->attachClick([](){
        Serial.println("Button L clicked");
		if (state->volume > 1)
        	radio->setVolume(state->volume - 1);
		else 
			radio->setMute(true);
	});

	btn_l->attachLongPressStart([](){
        Serial.println("Button L long press");
        radio->seekDown();
    });

    btn_m->attachClick([](){
        Serial.println("Button M clicked");
        radio->setMono(!radio->getMono());
    });

    btn_r->attachClick([](){
        Serial.println("Button R clicked");
		if (radio->getMute())
			 radio->setMute(false);
        radio->setVolume(state->volume + 1);
    });

	btn_r->attachLongPressStart([](){
        Serial.println("Button R long press");
        radio->seekUp();
    });
#endif

	startWifi();
}

void loop()
{
	server->loop();

#ifdef ENABLE_BUTTONS
	btn_l->tick();
    btn_m->tick();
    btn_r->tick();
#endif
}

void rdsCallBack(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4)
{
	logger->println("RDS: ");
	logger->println(block1);
	logger->println(block2);
	logger->println(block3);
	logger->println(block4);
	logger->println("-----");
}