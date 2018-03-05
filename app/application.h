#include <user_config.h>
#include <SmingCore.h>

#include "IRadio.h"

const int PIN_SCL = 5;
const int PIN_SDA = 4;

const RADIO_BAND FIX_BAND = RADIO_BAND_FM;
const RADIO_FREQ FIX_STATION = 9920;

const int WM_UNKNOWN   = 0;
const int WM_VOLUME    = 1;
const int WM_MONO      = 2;
const int WM_BAND      = 3;
const int WM_FREQ      = 4;
const int WM_SEEK_UP   = 5;
const int WM_SEEK_DOWN = 6;
const int WM_BASS_BOOST= 7;
const int WM_POWER     = 8;

struct RouteDefinition {
    String path;
    HttpPathDelegate callback;
} ;

struct WsMessageType {
    int id;
    String value;
} ;

struct RadioState {
	int volume;
	bool mono;
    bool power;
    bool bassBoost;
	RADIO_BAND band;
	RADIO_FREQ freq; 
} ;

void wifiConnected();
void wifiConnectFailed();
void startWebServer(HttpServer *srv);

void wsConnected(WebSocket &socket);
void wsMessageReceived(WebSocket &socket, const String &message);

void sendCapabilities();
void sendUpdate();

void onIndex(HttpRequest &request, HttpResponse &response);
void onFile(HttpRequest &request, HttpResponse &response);

void registerAllRoutes(HttpServer *srv);
int  getMessageId(String messageValue);
void updateState(IRadio *service);

void rdsCallBack(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4);