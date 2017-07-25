#include "application.h"

#define WIFI_SSID "wifi-12-private"
#define WIFI_PWD "9263777101"

#define SERIAL_DEBUG 0 // GPIO2

HttpServer server;
void onNtpReceive(NtpClient &client, time_t timestamp);

Timer printTimer;

int totalActiveSockets = 0;
int mute = 0;
int mixing = 1;
int source = 0;
int enhance = 0;
int power = 0;

float frequency = 0.0;

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

void onLcd(HttpRequest &request, HttpResponse &response)
{
	int y = request.getQueryParameter("y").toInt();
	String text = request.getQueryParameter("text");

	JsonObjectStream *stream = new JsonObjectStream();
	JsonObject &json = stream->getRoot();
	json["status"] = (bool)true;
	response.setAllowCrossDomainOrigin("*");
	response.setHeader("Access-Control-Allow-Methods", "GET,POST,PUT,DELETE,OPTIONS");
	response.setHeader("Access-Control-Allow-Headers", "Content-Type, Access-Control-Allow-Headers, Authorization, X-Request, X-Request, X-Requested-With");
	response.sendJsonObject(stream);
}

void setVolume(int channel, int val)
{
	int value = 74 - val;
	int x10 = 0;
	int x1 = 0;

	if (channel == CHAN_ALL)
	{

		volumeALLCH = value;
		volumeFR = value;
		volumeFL = value;
		volumeRR = value;
		volumeRL = value;
		volumeCEN = value;
		volumeSW = value;
		volumeALLCH = value;

		x10 = PT2258_ALLCH_10DB + (value / 10);
		x1 = PT2258_ALLCH_1DB + (value % 10);
	}
	else if (channel == CHAN_FR)
	{
		volumeFR = value;
		x10 = PT2258_FR_10DB + (value / 10);
		x1 = PT2258_FR_1DB + (value % 10);
	}
	else if (channel == CHAN_FL)
	{
		volumeFL = value;
		x10 = PT2258_FL_10DB + (value / 10);
		x1 = PT2258_FL_1DB + (value % 10);
	}
	else if (channel == CHAN_RR)
	{
		volumeRR = value;
		x10 = PT2258_RR_10DB + (value / 10);
		x1 = PT2258_RR_1DB + (value % 10);
	}
	else if (channel == CHAN_RL)
	{
		volumeRL = value;
		x10 = PT2258_RL_10DB + (value / 10);
		x1 = PT2258_RL_1DB + (value % 10);
	}
	else if (channel == CHAN_CEN)
	{
		volumeCEN = value;
		x10 = PT2258_CEN_10DB + (value / 10);
		x1 = PT2258_CEN_1DB + (value % 10);
	}
	else if (channel == CHAN_SW)
	{
		volumeSW = value;
		x10 = PT2258_SW_10DB + (value / 10);
		x1 = PT2258_SW_1DB + (value % 10);
	}

	Wire.beginTransmission(PT2258_ADDRESS);
	Wire.write(x1);
	Wire.write(x10);
	Wire.endTransmission();
}

void onVolume(HttpRequest &request, HttpResponse &response)
{

	int channel = request.getQueryParameter("channel").toInt();
	int value = request.getQueryParameter("value").toInt();

	setVolume(channel, value);

	JsonObjectStream *stream = new JsonObjectStream();
	JsonObject &json = stream->getRoot();
	json["status"] = (bool)true;
	response.setAllowCrossDomainOrigin("*");
	response.setHeader("Access-Control-Allow-Methods", "GET,POST,PUT,DELETE,OPTIONS");
	response.setHeader("Access-Control-Allow-Headers", "Content-Type, Access-Control-Allow-Headers, Authorization, X-Request, X-Request, X-Requested-With");

	response.sendJsonObject(stream);
}

void setSource(int sourc)
{
	source = sourc;
	int sourceID = 0;

	if (source == 0)
	{
		sourceID = 0xC7;
	}
	else if (source == 1)
	{
		sourceID = 0xCB;
	}
	else if (source == 2)
	{
		sourceID = 0xCA;
	}
	else if (source == 3)
	{
		sourceID = 0xC9;
	}
	else if (source == 4)
	{
		sourceID = 0xC8;
	}

	if (sourceID)
	{
		Wire.beginTransmission(PT2323_ADDRESS);
		Wire.write(DEVICE_REG_MODE1);
		Wire.write(sourceID);
		Wire.endTransmission();
	}
}

void onSource(HttpRequest &request, HttpResponse &response)
{
	int source = request.getQueryParameter("source").toInt();
	setSource(source);

	JsonObjectStream *stream = new JsonObjectStream();
	JsonObject &json = stream->getRoot();
	json["status"] = (bool)true;
	json["source"] = source;
	response.setAllowCrossDomainOrigin("*");
	response.setHeader("Access-Control-Allow-Methods", "GET,POST,PUT,DELETE,OPTIONS");
	response.setHeader("Access-Control-Allow-Headers", "Content-Type, Access-Control-Allow-Headers, Authorization, X-Request, X-Request, X-Requested-With");
	response.sendJsonObject(stream);
}

void onState(HttpRequest &request, HttpResponse &response)
{
	//systemTime = SystemClock.getSystemTimeString();
	int freq = request.getQueryParameter("value").toInt();
	JsonObjectStream *stream = new JsonObjectStream();
	JsonObject &json = stream->getRoot();
	json["status"] = (bool)true;
	json["time"] = SystemClock.now().toUnixTime();
	json["mute"] = mute;
	json["source"] = source;
	json["mixing"] = mixing;
	json["enhance"] = enhance;
	json["volumeFR"] = 74 - volumeFR;
	json["volumeFL"] = 74 - volumeFL;
	json["volumeRR"] = 74 - volumeRR;
	json["volumeRL"] = 74 - volumeRL;
	json["volumeCEN"] = 74 - volumeCEN;
	json["volumeSW"] = 74 - volumeSW;
	json["frequency"] = frequency;
	json["volumeALLCH"] = 74 - volumeALLCH;
	json["power"] = power;
	response.setAllowCrossDomainOrigin("*");
	response.setHeader("Access-Control-Allow-Methods", "GET,POST,PUT,DELETE,OPTIONS");
	response.setHeader("Access-Control-Allow-Headers", "Content-Type, Access-Control-Allow-Headers, Authorization, X-Request, X-Request, X-Requested-With");
	response.sendJsonObject(stream);
}

void setPower(int state)
{
	power = state;
}

void onPower(HttpRequest &request, HttpResponse &response)
{
	setPower(!power);

	JsonObjectStream *stream = new JsonObjectStream();
	JsonObject &json = stream->getRoot();
	json["status"] = (bool)true;
	json["state"] = (bool)power;
	response.setAllowCrossDomainOrigin("*");
	response.setHeader("Access-Control-Allow-Methods", "GET,POST,PUT,DELETE,OPTIONS");
	response.setHeader("Access-Control-Allow-Headers", "Content-Type, Access-Control-Allow-Headers, Authorization, X-Request, X-Request, X-Requested-With");
	response.sendJsonObject(stream);
}

void setMmute(int mut)
{
	mute = mut;
	if (mut == 1)
	{
		Wire.beginTransmission(PT2323_ADDRESS);
		Wire.write(DEVICE_REG_MODE1);
		Wire.write(0xFF);
		Wire.endTransmission();
	}
	else
	{
		Wire.beginTransmission(PT2323_ADDRESS);
		Wire.write(DEVICE_REG_MODE1);
		Wire.write(0xFE);
		Wire.endTransmission();
	}
}

void onMute(HttpRequest &request, HttpResponse &response)
{
	setMmute(!mute);

	JsonObjectStream *stream = new JsonObjectStream();
	JsonObject &json = stream->getRoot();
	json["status"] = (bool)true;
	json["state"] = (bool)mute;
	response.setAllowCrossDomainOrigin("*");
	response.setHeader("Access-Control-Allow-Methods", "GET,POST,PUT,DELETE,OPTIONS");
	response.setHeader("Access-Control-Allow-Headers", "Content-Type, Access-Control-Allow-Headers, Authorization, X-Request, X-Request, X-Requested-With");
	response.sendJsonObject(stream);
}

void setEnhance(int enhanc)
{
	enhance = enhanc;
	if (enhance)
	{
		Wire.beginTransmission(PT2323_ADDRESS);
		Wire.write(DEVICE_REG_MODE1);
		Wire.write(0xD0);
		Wire.endTransmission();
	}
	else
	{
		Wire.beginTransmission(PT2323_ADDRESS);
		Wire.write(DEVICE_REG_MODE1);
		Wire.write(0xD1);
		Wire.endTransmission();
	}
}

void onEnhance(HttpRequest &request, HttpResponse &response)
{
	setEnhance(!enhance);
	JsonObjectStream *stream = new JsonObjectStream();
	JsonObject &json = stream->getRoot();
	json["status"] = (bool)true;
	json["state"] = (bool)enhance;
	response.setAllowCrossDomainOrigin("*");
	response.setHeader("Access-Control-Allow-Methods", "GET,POST,PUT,DELETE,OPTIONS");
	response.setHeader("Access-Control-Allow-Headers", "Content-Type, Access-Control-Allow-Headers, Authorization, X-Request, X-Request, X-Requested-With");

	response.sendJsonObject(stream);
}

void setMixing(int mixin)
{
	mixing = mixin;
	if (mixing)
	{
		Wire.beginTransmission(PT2323_ADDRESS);
		Wire.write(DEVICE_REG_MODE1);
		Wire.write(0x90);
		Wire.endTransmission();
	}
	else
	{
		Wire.beginTransmission(PT2323_ADDRESS);
		Wire.write(DEVICE_REG_MODE1);
		Wire.write(0x91);
		Wire.endTransmission();
	}
}

void onMixing(HttpRequest &request, HttpResponse &response)
{
	setMixing(!mixing);
	JsonObjectStream *stream = new JsonObjectStream();
	JsonObject &json = stream->getRoot();
	json["status"] = (bool)true;
	json["state"] = (bool)mixing;
	response.setAllowCrossDomainOrigin("*");
	response.setHeader("Access-Control-Allow-Methods", "GET,POST,PUT,DELETE,OPTIONS");
	response.setHeader("Access-Control-Allow-Headers", "Content-Type, Access-Control-Allow-Headers, Authorization, X-Request, X-Request, X-Requested-With");
	response.sendJsonObject(stream);
}

void setFrequency(float freq)
{
	frequency = freq;
	unsigned int frequencyB = 4 * (frequency * 1000000 + 225000) / 32768;
	byte frequencyH = frequencyB >> 8;
	byte frequencyL = frequencyB & 0XFF;
	Wire.beginTransmission(0x60);
	Wire.write(frequencyH);
	Wire.write(frequencyL);
	Wire.write(0xB0);
	Wire.write(0x10);
	Wire.write(0x00);
	Wire.endTransmission();
}

void onTune(HttpRequest &request, HttpResponse &response)
{
	setFrequency(request.getQueryParameter("freq").toFloat());

	JsonObjectStream *stream = new JsonObjectStream();
	JsonObject &json = stream->getRoot();
	json["status"] = (bool)true;
	response.setAllowCrossDomainOrigin("*");
	response.setHeader("Access-Control-Allow-Methods", "GET,POST,PUT,DELETE,OPTIONS");
	response.setHeader("Access-Control-Allow-Headers", "Content-Type, Access-Control-Allow-Headers, Authorization, X-Request, X-Request, X-Requested-With");
	response.sendJsonObject(stream);
}

int connectTimeout = 0;

void onPrintSystemTime()
{
	if (!WifiStation.isConnected())
	{
		if (connectTimeout > 2)
		{
			System.restart();
			connectTimeout = 0;
		}
	}
}

void sendUpdate()
{
	StaticJsonBuffer<300> sendJsonBuffer;
	JsonObject &json = sendJsonBuffer.createObject();
	json["type"] = "state";
	json["count"] = totalActiveSockets;
	json["time"] = SystemClock.now().toUnixTime();
	json["mute"] = mute;
	json["source"] = source;
	json["mixing"] = mixing;
	json["enhance"] = enhance;
	json["volumeFR"] = 74 - volumeFR;
	json["volumeFL"] = 74 - volumeFL;
	json["volumeRR"] = 74 - volumeRR;
	json["volumeRL"] = 74 - volumeRL;
	json["volumeCEN"] = 74 - volumeCEN;
	json["volumeSW"] = 74 - volumeSW;
	json["frequency"] = frequency;
	json["volumeALLCH"] = 74 - volumeALLCH;
	json["power"] = power;

	String jsonString;
	json.printTo(jsonString);

	WebSocketsList &clients = server.getActiveWebSockets();
	for (int i = 0; i < clients.count(); i++)
	{
		clients[i].sendString(jsonString);
	}
}

void wsConnected(WebSocket &socket)
{
	totalActiveSockets++;
	sendUpdate();
}

void wsMessageReceived(WebSocket &socket, const String &message)
{
	WebSocketsList &clients = server.getActiveWebSockets();

	StaticJsonBuffer<200> jsonBuffer;
	JsonObject &root = jsonBuffer.parseObject(message);
	String actionName = root["name"].asString();

	if (actionName == "enhance")
	{
		setEnhance(!enhance);
	}
	else if (actionName == "power")
	{
		setPower(!power);
	}
	else if (actionName == "mute")
	{
		setMmute(!mute);
	}
	else if (actionName == "mixing")
	{
		setMixing(!mixing);
	}
	else if (actionName == "frequency")
	{
		setFrequency(root["val"]);
	}
	else if (actionName == "source")
	{
		setSource(root["val"]);
	}
	else if (actionName == "volumeFR")
	{
		setVolume(CHAN_FR, root["val"]);
	}
	else if (actionName == "volumeFL")
	{
		setVolume(CHAN_FL, root["val"]);
	}
	else if (actionName == "volumeRR")
	{
		setVolume(CHAN_RR, root["val"]);
	}
	else if (actionName == "volumeRL")
	{
		setVolume(CHAN_RL, root["val"]);
	}
	else if (actionName == "volumeCEN")
	{
		setVolume(CHAN_CEN, root["val"]);
	}
	else if (actionName == "volumeSW")
	{
		setVolume(CHAN_SW, root["val"]);
	}
	else if (actionName == "volumeALLCH")
	{
		setVolume(CHAN_ALL, root["val"]);
	}
	else if (actionName == "volumeSW")
	{
		setVolume(CHAN_SW, root["val"]);
	}

	sendUpdate();
	Serial.printf("WebSocket message received:\r\n%s\r\n", actionName.c_str());
}

void wsBinaryReceived(WebSocket &socket, uint8_t *data, size_t size)
{
	if (SERIAL_DEBUG)
		Serial.printf("Websocket binary data recieved, size: %d\r\n", size);
}

void wsDisconnected(WebSocket &socket)
{
	totalActiveSockets--;
}

void startmDNS()
{
	struct mdns_info info;
	struct ip_info ipconfig;

	wifi_get_ip_info(STATION_IF, &ipconfig);
	info.host_name = (char *)"music";
	info.ipAddr = ipconfig.ip.addr;
	info.server_name = (char *)"iot";
	info.server_port = 80;
	info.txt_data[0] = (char *)"version = now";
	info.txt_data[1] = (char *)"user1 = data1";
	info.txt_data[2] = (char *)"user2 = data2";
// @@	espconn_mdns_init(&info);
}

void startWebServer()
{
	server.listen(80);
	server.addPath("/", onIndex);
	server.addPath("/lcd", onLcd);
	server.addPath("/volume", onVolume);
	server.addPath("/mute", onMute);
	server.addPath("/source", onSource);
	server.addPath("/state", onState);
	server.addPath("/power", onPower);
	server.addPath("/tune", onTune);
	server.addPath("/mixing", onMixing);
	server.addPath("/enhance", onEnhance);

	// Web Sockets configuration
	server.enableWebSockets(true);
	server.setWebSocketConnectionHandler(wsConnected);
	server.setWebSocketMessageHandler(wsMessageReceived);
	server.setWebSocketBinaryHandler(wsBinaryReceived);
	server.setWebSocketDisconnectionHandler(wsDisconnected);

	server.setDefaultHandler(onFile);

	Serial.println("\r\n=== WEB SERVER STARTED ===");
	Serial.println(WifiStation.getIP());
}

void connectOk()
{
	startmDNS();
	Serial.println("I'm CONNECTED");
	startWebServer();
}

void connectFail()
{
	debugf("I'm NOT CONNECTED!");
	WifiStation.waitConnection(connectOk, 30, connectFail); // Repeat and check again
}

void init()
{
	spiffs_mount();
	Serial.begin(115200);
	Serial.systemDebugOutput(false);

	Wire.begin();

	SystemClock.setTimeZone(3);
	printTimer.initializeMs(1000 * 60, onPrintSystemTime).start();

	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Enable debug output to serial
	WifiStation.enable(true);
	WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiAccessPoint.enable(false);

	Wire.beginTransmission(PT2258_ADDRESS);
	Wire.write(0xC0);
	Wire.endTransmission();

	// Run our method when station was connected to AP
	WifiStation.waitConnection(connectOk, 30, connectFail);
}