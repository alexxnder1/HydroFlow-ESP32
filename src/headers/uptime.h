#pragma once
#include <string>
#include <esp32-hal.h>
#include <ESPAsyncWebServer.h>

extern AsyncWebServer server;

namespace UptimeManager {
    extern std::string uptime;
    
    unsigned long GetUptime();
    void handleUptime(AsyncWebServerRequest* request);
    void SetRoutes();
}