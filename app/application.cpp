#include "application.h"

#define WIFI_SSID "wifi-12-private"
#define WIFI_PWD "9263777101"

HttpServer server;
RadioState state;

const int SERVICES_LEN = 1;

IRadio *radioServices[SERVICES_LEN] = { 
	// new RadioScreen(),
	new RadioModule() 
};

RouteDefinition routes[] = {
	{ "/", onIndex }
};

WsMessageType messageDefs[] = {
	{ WM_VOLUME, "volume" },
	{ WM_MONO  , "mono" },
	{ WM_BAND  , "band" },
	{ WM_FREQ  , "freq" }
};

void init()
{
	spiffs_mount();

	Serial.begin(SERIAL_BAUD_RATE);
	Serial.systemDebugOutput(false);

	Wire.begin(PIN_SCL, PIN_SDA);

	WifiStation.enable(true);
	WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiStation.waitConnection(wifiConnected, 30, wifiConnectFailed);

	for (int i = 0; i < SERVICES_LEN; i++) {
		IRadio *x = radioServices[i];
		x->init();
	}
}
 
void wifiConnected()
{
	Serial.println("WiFi ONLINE");
	startWebServer(&server);
}

void wifiConnectFailed()
{
	debugf("WiFi OFFLINE");
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
	sendUpdate();
}

void sendUpdate()
{
	StaticJsonBuffer<300> sendJsonBuffer;
	JsonObject &json = sendJsonBuffer.createObject();

	json["type"] = "state";
	json["volume"] = state.volume;
	json["mono"] = state.mono;
	json["band"] = (int)state.band;
	json["freq"] = state.freq;

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

	switch (messageId) {
		case WM_VOLUME:
			{
				int res = 0;
				for (int i = 0; i < SERVICES_LEN; i++) {
					IRadio *service = radioServices[i];
					res = service->setVolume(x);
				}
				state.volume = res;
			}
			break;
		case WM_MONO:
			{
				bool res = 0;
				for (int i = 0; i < SERVICES_LEN; i++) {
					IRadio *service = radioServices[i];
					res = service->setMono(x);
				}
				state.mono = res;
			}
			break;
		case WM_BAND:
			{
				RADIO_BAND res = RADIO_BAND_NONE;
				for (int i = 0; i < SERVICES_LEN; i++) {
					IRadio *service = radioServices[i];
					res = service->setBand((RADIO_BAND)x);
				}
				state.band = res;
			}
			break;
		case WM_FREQ:
			{
				RADIO_FREQ res = 0;
				for (int i = 0; i < SERVICES_LEN; i++) {
					IRadio *service = radioServices[i];
					res = service->setFrequency(x);
				}
				state.freq = res;
			}
			break;
	}

	sendUpdate();
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

void registerAllRoutes(HttpServer *srv) {
	int routesCount = sizeof(routes) / sizeof(RouteDefinition);
	for (int i = 0; i < routesCount; i++) {
		srv->addPath( routes[i].path, routes[i].callback );
		Serial.print("Registered route: ");
		Serial.println(routes[i].path);
	}
}

int getMessageId(String messageValue) {
	int count = sizeof(messageDefs) / sizeof(WsMessageType);
	int result = 0;
	for (int i = 0; i < count; i++)
		if (messageValue == messageDefs[i].value)
			return messageDefs[i].id;
	return WM_UNKNOWN;
}