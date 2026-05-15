#include "./headers/web_socket.h"

namespace WebSocket {
    AsyncWebSocket ws("/ws");
    
    void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
    {
        if (type == WS_EVT_CONNECT) {
            Serial.printf("\n[WebSocket] Client connected with IP: %s, ID: %u\n", client->remoteIP().toString().c_str(), client->id());
            client->text("[WebSocket] Connection established successfully!");
        } 
        else if (type == WS_EVT_DISCONNECT) {
            Serial.printf("\n[WebSocket] Client disconnected. ID client: %u\n", client->id());
        }
    }
        
    void Setup()
    {
        ws.onEvent(onWsEvent);
        server.addHandler(&ws);
    }
}