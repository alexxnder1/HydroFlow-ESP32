#pragma once
#include <string>
#include <ESPAsyncWebServer.h>

extern AsyncWebServer server;

namespace TimestampManager {
    extern std::string timestamp;
    void handleGetTime(AsyncWebServerRequest *request);
    void SetRoutes();
}