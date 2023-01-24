#include "webserver.h"
#include "logger.h"
#include "radio.h"
#include <ArduinoJson.h>

WebServer::WebServer(RadioState* state)
{
    _state = state;
    _server = new ESP8266WebServer(HTTP_PORT);
    _ws = new WebSocketsServer(WS_PORT);
    _ticker = new Ticker();
}

void WebServer::init()
{
    fsOK = fileSystem->begin();
    if (!fsOK)
        logger->println("Failed to mount SPIFFS");

    logger->println("Starting webserver");

    _server->on("/", HTTP_GET, [&]()
    { 
        handleFileRead("/index.html"); 
    });

    _server->onNotFound([&]()
    {
        if (!fsOK)
        {
            return replyServerError(FPSTR(FS_INIT_ERROR));
        }

        String uri = ESP8266WebServer::urlDecode(_server->uri()); // required to read paths with blanks

        if (handleFileRead(uri))
        {
            return;
        }

        // Dump debug data
        String message;
        message.reserve(100);
        message = F("Error: File not found\n\nURI: ");
        message += uri;
        message += F("\nMethod: ");
        message += (_server->method() == HTTP_GET) ? "GET" : "POST";
        message += F("\nArguments: ");
        message += _server->args();
        message += '\n';
        for (uint8_t i = 0; i < _server->args(); i++)
        {
            message += F(" NAME:");
            message += _server->argName(i);
            message += F("\n VALUE:");
            message += _server->arg(i);
            message += '\n';
        }
        message += "path=";
        message += _server->arg("path");
        message += '\n';
        logger->print(message);

        return replyNotFound(message); 
    });

    _ws->onEvent([&](uint8_t num, WStype_t type, uint8_t *payload, size_t length)
    {
        switch(type) {
        case WStype_DISCONNECTED:
        {
            logger->printf("[%u] Disconnected!\n", num);
        }
        break;

        case WStype_CONNECTED:
        {
            IPAddress ip = _ws->remoteIP(num);
            logger->printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

            sendCapabilities();
            sendUpdate();
        }
        break;

        case WStype_TEXT:
        {
            #if (DEBUG > 1)
            logger->printf("[%u] << : %s\n", num, payload);
            #endif
            StaticJsonDocument<JSON_MAX_SIZE> doc;
            deserializeJson(doc, payload);
            String actionName = doc["name"];
            String actionValue = doc["value"];
            int x = doc["value"];
            int messageId = getMessageId(actionName);
            switch (messageId)
            {
                case WM_POWER:
                {
                    logger->print("WM_POWER -> ");
                    logger->println(x);
                    _state->setPower(x);
                }
                case WM_VOLUME:
                {
                    logger->print("WM_VOLUME -> ");
                    logger->println(x);
                    _state->radio->setVolume(x);
                    _state->volume = x;
                }
                break;
                case WM_MONO:
                {
                    logger->print("WM_MONO -> ");
                    logger->println(x);
                    _state->radio->setMono(x);
                    _state->mono = x;
                }
                break;
                case WM_BAND:
                {
                    _state->radio->setBand((RADIO_BAND)x);
                    _state->band = (RADIO_BAND)x;
                }
                break;
                case WM_FREQ:
                {
                    logger->print("WM_FREQ -> ");
                    logger->println(x);
                    _state->radio->setFrequency(x);
                    _state->freq = x;
                }
                break;
                case WM_SEEK_UP:
                {
                    logger->print("WM_SEEK_UP -> ");
                    logger->println(x);
                    _state->radio->seekUp();
                }
                break;
                case WM_SEEK_DOWN:
                {
                    logger->print("WM_SEEK_DOWN -> ");
                    logger->println(x);
                    _state->radio->seekDown();
                }
                break;
                case WM_BASS_BOOST:
                {
                    logger->print("WM_BASS_BOOST -> ");
                    logger->println(x);
                    _state->radio->setBassBoost(x);
                    _state->bassBoost = x;
                }
                break;
            }
        }
        break;

        case WStype_ERROR:
        default:
            ;
        }
    });

    _server->begin();
    logger->println("Http web server started");

    _ws->begin();
    logger->println("WS web server started");

    _ticker->attach_ms(UPDATE_STATE_PERIOD_MS, [&]() {
        sendUpdate();
    });
}

int WebServer::getMessageId(String messageValue)
{
	uint8_t count = sizeof(messageDefs) / sizeof(WsMessageType);
	for (uint8_t i = 0; i < count; i++)
		if (messageValue == messageDefs[i].value)
			return messageDefs[i].id;
	return WM_UNKNOWN;
}

bool WebServer::handleFileRead(String path)
{
    if (!fsOK)
    {
        replyServerError(FPSTR(FS_INIT_ERROR));
        return true;
    }

    String contentType = mime::getContentType(path);

    if (!fileSystem->exists(path))
    {
        // File not found, try gzip version
        path = path + ".gz";
    }

    if (fileSystem->exists(path))
    {
        File file = fileSystem->open(path, "r");
        size_t bytesSent = _server->streamFile(file, contentType);
        if (bytesSent != file.size())
        {
            logger->printf("Sent less data than expected (%s): %d sent, %d expected\n", path.c_str(), bytesSent, file.size());
        }
        file.close();
        return true;
    }

    return false;
}

void WebServer::replyNotFound(String msg)
{
    _server->send(404, FPSTR(TEXT_PLAIN), msg);
}

void WebServer::replyServerError(String msg)
{
    logger->println(msg);
    _server->send(500, FPSTR(TEXT_PLAIN), msg + "\r\n");
}

void WebServer::loop(void)
{
    _server->handleClient();
    _ws->loop();
}

void WebServer::sendCapabilities()
{
	StaticJsonDocument<JSON_MAX_SIZE> json;
	RadioCapabilities caps = _state->caps;
	json["type"] = "capabilities";
	json["volume"] = caps.volume;
	json["bass"] = caps.bassBoost;
	json["rds"] = caps.rds;
	String jsonString;
	serializeJson(json, jsonString);
    _ws->broadcastTXT(jsonString);
    #if (DEBUG > 1)
    logger->printf("[A] >> : %s\n", jsonString.c_str());
    #endif
}

void WebServer::sendUpdate()
{
	StaticJsonDocument<JSON_MAX_SIZE> json;
	json["type"] = "state";
	json["volume"] = _state->volume;
	json["mono"] = _state->mono;
	json["band"] = (int)_state->band;
	json["freq"] = _state->freq;
	json["bass"] = _state->bassBoost;
	json["power"] = _state->power;
	String jsonString;
    serializeJson(json, jsonString);
    _ws->broadcastTXT(jsonString);
    #if (DEBUG > 1)
    logger->printf("[A] >> : %s\n", jsonString.c_str());
    #endif
}