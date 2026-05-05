#pragma once
#include <string>
#include <ESPAsyncWebServer.h>

extern AsyncWebServer server;

namespace TimestampManager {
    extern std::string startTimestamp;
    void handleGetTime(AsyncWebServerRequest *request);
    void SetRoutes();
}