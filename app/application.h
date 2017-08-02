#include <user_config.h>
#include <SmingCore.h>

#include "IRadio.h"
#include "RadioModule.h"
#include "RadioScreen.h"

const int PIN_SCL = 5;
const int PIN_SDA = 4;

const int WM_UNKNOWN = 0;
const int WM_VOLUME  = 1;
const int WM_MONO    = 2;
const int WM_BAND    = 3;
const int WM_FREQ    = 4;

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
	RADIO_BAND band;
	RADIO_FREQ freq; 
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
int getMessageId(String messageValue);
void updateState(IRadio *service);