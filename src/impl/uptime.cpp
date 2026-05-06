#include "./headers/uptime.h"
#include "./headers/timestamp.h"

namespace UptimeManager {
    std::string uptime;
    
    unsigned long GetUptime() {
        return millis();
    }

    void handleUptime(AsyncWebServerRequest* request)
    {
        char json[64];
        unsigned long currentUptime = GetUptime() / 1000;
        snprintf(json, sizeof(json), "{\"uptime\":\"%lu\"}", currentUptime);
        request->send(200, "application/json", json);
    }

    void SetRoutes()
    {
        server.on("/get_uptime", HTTP_GET, handleUptime);
    }
}