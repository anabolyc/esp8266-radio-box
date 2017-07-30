#include <user_config.h>
#include <SmingCore.h>

#include "IRadio.h"
#include "RadioModule.h"
#include "RadioScreen.h"

struct RouteDefinition {
    String path;
    HttpPathDelegate callback;
} ;

void wifiConnected();
void wifiConnectFailed();
void startWebServer(HttpServer *srv);

void wsConnected(WebSocket &socket);
void wsMessageReceived(WebSocket &socket, const String &message);

void sendUpdate();

void onIndex(HttpRequest &request, HttpResponse &response);
void onFile(HttpRequest &request, HttpResponse &response);

void registerAllRoutes(HttpServer *srv);

/*
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

void onState(HttpRequest &request, HttpResponse &response)
{
	int freq = request.getQueryParameter("value").toInt();
	JsonObjectStream *stream = new JsonObjectStream();
	JsonObject &json = stream->getRoot();
	json["status"] = (bool)true;
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
*/