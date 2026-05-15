#pragma once
#include <ESPAsyncWebServer.h>

extern AsyncWebServer server; 

namespace WebSocket {
    extern AsyncWebSocket ws; 

    void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
    void Setup();
}