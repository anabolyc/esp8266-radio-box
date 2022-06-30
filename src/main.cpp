#include <Arduino.h>
#include <WiFiManager.h>
#include <TFT_eSPI.h>

WiFiManager wifiManager;

#include "statusled.h"
#include "webserver.h"
#include "tftdisplay.h"
#include "logger.h"
#include "main.h"

StatusLed *statusLed = new StatusLed();
Radio *radio = new Radio();
RadioState *state = new RadioState(RadioCaps, radio);
TFT_eSPI* tft = new TFT_eSPI();
TftDisplay* tftdisplay = new TftDisplay(tft, state);
WebServer *server = new WebServer(state);

const RADIO_BAND FIX_BAND = RADIO_BAND_FM;
const RADIO_FREQ FIX_STATION = 9920;

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

	if (!wifiManager.autoConnect())
	{
		logger->println("Failed to connect and hit timeout");
		ESP.restart();
	}
	else
	{
		logger->printf("Connected: ");
		logger->println(WiFi.localIP());
		server->init();
		statusLed->setState(ApplicationState::READY);
	}
}

unsigned long testText()
{
	tft->fillScreen(TFT_BLACK);
	unsigned long start = micros();
	tft->setCursor(0, 0);
	tft->setTextColor(TFT_WHITE);
	tft->setTextSize(1);
	tft->println("Hello World!");
	tft->setTextColor(TFT_YELLOW);
	tft->setTextSize(2);
	tft->println(1234.56);
	tft->setTextColor(TFT_RED);
	tft->setTextSize(3);
	tft->println(0xDEADBEEF, HEX);
	tft->println();
	tft->setTextColor(TFT_GREEN);
	tft->setTextSize(5);
	tft->println("Groop");
	tft->setTextSize(2);
	tft->println("I implore thee,");
	// tft->setTextColor(TFT_GREEN,TFT_BLACK);
	tft->setTextSize(1);
	tft->println("my foonting turlingdromes.");
	tft->println("And hooptiously drangle me");
	tft->println("with crinkly bindlewurdles,");
	tft->println("Or I will rend thee");
	tft->println("in the gobberwarts");
	tft->println("with my blurglecruncheon,");
	tft->println("see if I don't!");
	return micros() - start;
}

void setup()
{
	logger->begin(SERIAL_BAUD);
	startWifi();

	Wire.begin(PIN_SDA, PIN_SCL);
	radio->init();
	radio->debugEnable(DEBUG > 0);
	delay(100);

	radio->setBand(FIX_BAND);
	radio->setFrequency(FIX_STATION);

	radio->setVolume(2);
	radio->setMono(false);
	radio->setMute(false);
	logger->println("Radio services started");
	// updateState(_radio);
	logger->println("Initialization finished");

	tftdisplay->init();

	// tft->init();
	// tft->fillScreen(TFT_BLACK);

	//testText();
}

void loop()
{
	server->loop();
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