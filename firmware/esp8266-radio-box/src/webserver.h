#pragma once

#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <FS.h>
#include <Ticker.h>

#include "radiostate.h"

#define HTTP_PORT 80
#define WS_PORT 81
#define JSON_MAX_SIZE 255
// TODO only update when state changes
#define UPDATE_STATE_PERIOD_MS 1000

static const char TEXT_PLAIN[] PROGMEM = "text/plain";
static const char FS_INIT_ERROR[] PROGMEM = "FS INIT ERROR";
static const char FILE_NOT_FOUND[] PROGMEM = "FileNotFound";

const int WM_UNKNOWN   = 0;
const int WM_VOLUME    = 1;
const int WM_MONO      = 2;
const int WM_BAND      = 3;
const int WM_FREQ      = 4;
const int WM_SEEK_UP   = 5;
const int WM_SEEK_DOWN = 6;
const int WM_BASS_BOOST= 7;
const int WM_POWER     = 8;

struct WsMessageType {
    int id;
    String value;
};

static WsMessageType messageDefs[] = {
	{WM_VOLUME, "volume"},
	{WM_MONO, "mono"},
	{WM_BAND, "band"},
	{WM_FREQ, "freq"},
	{WM_SEEK_UP, "seek_up"},
	{WM_SEEK_DOWN, "seek_down"},
	{WM_BASS_BOOST, "bass_boost"},
	{WM_POWER, "power"}
};

class WebServer {
    public:
        WebServer(RadioState*);
        void init(void);
        void loop(void);

    private:
        RadioState* _state;
        ESP8266WebServer* _server;
        WebSocketsServer* _ws;
        Ticker* _ticker;
        FS* fileSystem = &SPIFFS;

        bool fsOK = false;

        static int getMessageId(String messageValue);
        bool handleFileRead(String path);
        void replyNotFound(String msg);
        void replyServerError(String msg);
        void sendUpdate();
        void sendCapabilities();
};