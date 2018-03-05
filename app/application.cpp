#include "application.h"
#include "TEA5767.h"

#define WIFI_SSID "wifi-12-private"
#define WIFI_PWD "9263777101"

HttpServer server;
RadioState state;

Timer wsClientTimer;

IRadio *_radio = new TEA5767();

RouteDefinition routes[] = {
	{"/", onIndex}};

WsMessageType messageDefs[] = {
	{WM_VOLUME, "volume"},
	{WM_MONO, "mono"},
	{WM_BAND, "band"},
	{WM_FREQ, "freq"},
	{WM_SEEK_UP, "seek_up"},
	{WM_SEEK_DOWN, "seek_down"},
	{WM_BASS_BOOST, "bass_boost"},
	{WM_POWER, "power"}};

void init()
{
	spiffs_mount();

	Serial.begin(SERIAL_BAUD_RATE);
	Serial.systemDebugOutput(false);

	Wire.begin(PIN_SCL, PIN_SDA);

	WifiStation.enable(true);
	WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiStation.waitConnection(wifiConnected, 30, wifiConnectFailed);

    //_radio->debugEnable(false);
    _radio->init();
    delay(100);
    _radio->setBand(FIX_BAND);
    _radio->setFrequency(FIX_STATION);
    //_radio->setVolume(1);
    _radio->setMono(false);

	Serial.println("Radio services started");

	updateState(_radio);
	Serial.println("Initialization finished");
}

void updateState(IRadio *service)
{
	state.power = service->getPower();
	state.volume = service->getVolume();
	state.mono = service->getMono();
	state.band = service->getBand();
	state.freq = service->getFrequency();
	state.bassBoost = service->getBassBoost();

	{
		RADIO_INFO info;
		service->getRadioInfo(&info);

		Serial.print("VOL: ");
		Serial.print(state.volume);
		Serial.print("  FRQ: ");
		Serial.print(state.freq);
		Serial.print("  PWR: ");
		Serial.print(state.power);

		Serial.print(info.rds ? " RDS" : " ---");
		Serial.print(info.tuned ? " TUNED" : " -----");
		Serial.print(info.stereo ? " STEREO" : "  MONO ");
		Serial.print("  RSSI: ");
		Serial.print(info.rssi);
		Serial.print("  S/N: ");
		Serial.print(info.snr);
	}

	{
		AUDIO_INFO info;
		service->getAudioInfo(&info);

		Serial.print(info.bassBoost ? " BASS" : " ----");
		Serial.print(info.mute ? " MUTE" : " ----");
		Serial.print(info.softmute ? " SOFTMUTE" : " --------");
		Serial.println();
	}
}

void wifiConnected()
{
	Serial.println("WiFi ONLINE");
	startWebServer(&server);
	wsClientTimer.initializeMs(1000, sendUpdate).start();
}

void wifiConnectFailed()
{
	Serial.println("WiFi OFFLINE");
	WifiStation.waitConnection(wifiConnected, 30, wifiConnectFailed);
}

void startWebServer(HttpServer *srv)
{
	srv->listen(80);
	registerAllRoutes(srv);

	srv->enableWebSockets(true);
	srv->setWebSocketConnectionHandler(wsConnected);
	srv->setWebSocketMessageHandler(wsMessageReceived);

	srv->setDefaultHandler(onFile);

	Serial.println("\r\n=== WEB SERVER STARTED ===");
	Serial.println(WifiStation.getIP());
}

void wsConnected(WebSocket &socket)
{
	sendCapabilities();
	sendUpdate();
}

void sendCapabilities()
{
	StaticJsonBuffer<300> sendJsonBuffer;
	JsonObject &json = sendJsonBuffer.createObject();

	RADIO_CAP caps = _radio->getCapabilities();

	json["type"] = "capabilities";
	json["volume"] = caps.volume;
	json["bass"] = caps.bassBoost;
	json["rds"] = caps.rds;

	String jsonString;
	json.printTo(jsonString);

	WebSocketsList &clients = server.getActiveWebSockets();
	for (int i = 0; i < clients.count(); i++)
		clients[i].sendString(jsonString);
}

void sendUpdate()
{
	StaticJsonBuffer<300> sendJsonBuffer;
	JsonObject &json = sendJsonBuffer.createObject();

	updateState(_radio);

	json["type"] = "state";
	json["volume"] = state.volume;
	json["mono"] = state.mono;
	json["band"] = (int)state.band;
	json["freq"] = state.freq;
	json["bass"] = state.bassBoost;
	json["power"] = state.power;

	String jsonString;
	json.printTo(jsonString);

	WebSocketsList &clients = server.getActiveWebSockets();
	for (int i = 0; i < clients.count(); i++)
		clients[i].sendString(jsonString);
}

void wsMessageReceived(WebSocket &socket, const String &message)
{
	WebSocketsList &clients = server.getActiveWebSockets();

	StaticJsonBuffer<200> jsonBuffer;
	JsonObject &root = jsonBuffer.parseObject(message);
	String actionName = root["name"].asString();
	String actionValue = root["value"].asString();
	int x = root["value"];
	int messageId = getMessageId(actionName);

	switch (messageId)
	{
	case WM_POWER:
	{
		Serial.print("WM_POWER -> ");
		Serial.println(x);
		_radio->setPower(x);
		state.power = x;
	}
	case WM_VOLUME:
	{
		Serial.print("WM_VOLUME -> ");
		Serial.println(x);
		_radio->setVolume(x);
		state.volume = x;
	}
	break;
	case WM_MONO:
	{
		Serial.print("WM_MONO -> ");
		Serial.println(x);
		_radio->setMono(x);
		state.mono = x;
	}
	break;
	case WM_BAND:
	{
		_radio->setBand((RADIO_BAND)x);
		state.band = (RADIO_BAND)x;
	}
	break;
	case WM_FREQ:
	{
		Serial.print("WM_FREQ -> ");
		Serial.println(x);
		_radio->setFrequency(x);
		state.freq = x;
	}
	break;
	case WM_SEEK_UP:
	{
		Serial.print("WM_SEEK_UP -> ");
		Serial.println(x);
		_radio->seekUp();
	}
	break;
	case WM_SEEK_DOWN:
	{
		Serial.print("WM_SEEK_DOWN -> ");
		Serial.println(x);
		_radio->seekDown();
	}
	break;
	case WM_BASS_BOOST:
	{
		Serial.print("WM_BASS_BOOST -> ");
		Serial.println(x);
		_radio->setBassBoost(x);
		state.bassBoost = x;
	}
	break;
	}
}

void onIndex(HttpRequest &request, HttpResponse &response)
{
	TemplateFileStream *tmpl = new TemplateFileStream("index.html");
	auto &vars = tmpl->variables();
	response.sendTemplate(tmpl);
}

void onFile(HttpRequest &request, HttpResponse &response)
{
	String file = request.getPath();
	if (file[0] == '/')
		file = file.substring(1);

	if (file[0] == '.')
		response.forbidden();
	else
	{
		response.setCache(60, true);
		response.sendFile(file);
	}
}

void registerAllRoutes(HttpServer *srv)
{
	int routesCount = sizeof(routes) / sizeof(RouteDefinition);
	for (int i = 0; i < routesCount; i++)
	{
		srv->addPath(routes[i].path, routes[i].callback);
		Serial.print("Registered route: ");
		Serial.println(routes[i].path);
	}
}

int getMessageId(String messageValue)
{
	int count = sizeof(messageDefs) / sizeof(WsMessageType);
	int result = 0;
	for (int i = 0; i < count; i++)
		if (messageValue == messageDefs[i].value)
			return messageDefs[i].id;
	return WM_UNKNOWN;
}

void rdsCallBack(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4)
{
	Serial.println("RDS: ");
	Serial.println(block1);
	Serial.println(block2);
	Serial.println(block3);
	Serial.println(block4);
	Serial.println("-----");
}