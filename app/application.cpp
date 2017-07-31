#include "application.h"

#define WIFI_SSID "wifi-12-private"
#define WIFI_PWD "9263777101"

HttpServer server;

const int SERVICES_LEN = 2;
IRadio *radioServices[SERVICES_LEN] = { 
	new RadioModule(), 
	new RadioScreen()
};

RouteDefinition routes[] = {
	{ "/", onIndex }
	/*
	server.addPath("/volume", onVolume);
	server.addPath("/mute", onMute);
	server.addPath("/state", onState);
	server.addPath("/power", onPower);
	server.addPath("/tune", onTune);
	server.addPath("/mixing", onMixing);
	server.addPath("/enhance", onEnhance);
	*/
};

WsMessageType messageDefs[] = {
	{ WM_VOLUME, "volume" }
};

int mute = 0;
int mixing = 1;
int source = 0;
int enhance = 0;
int power = 0;
float frequency = 0.0;

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
	json["mute"] = mute;
	json["source"] = source;
	json["mixing"] = mixing;
	json["enhance"] = enhance;
	json["frequency"] = frequency;
	json["power"] = power;

	String jsonString;
	json.printTo(jsonString);

	WebSocketsList &clients = server.getActiveWebSockets();
	for (int i = 0; i < clients.count(); i++)
	{
		clients[i].sendString(jsonString);
	}
}

void wsMessageReceived(WebSocket &socket, const String &message)
{
	WebSocketsList &clients = server.getActiveWebSockets();

	StaticJsonBuffer<200> jsonBuffer;
	JsonObject &root = jsonBuffer.parseObject(message);
	String actionName = root["name"].asString();
	String actionValue = root["value"].asString();
	int messageId = getMessageId(actionName);

	switch (messageId) {
		case WM_VOLUME:
			Serial.print("volume: ");
			Serial.println(actionValue);
			break;
	}

	sendUpdate();
	Serial.printf("WebSocket message received:\r\n%s\r\n", actionName.c_str());
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