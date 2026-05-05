#include <string>
#include "./headers/timestamp.h"
#include <ESPAsyncWebServer.h>
#include <iostream>
#include <sys/time.h>

namespace TimestampManager {
    std::string startTimestamp = "";

    void handleGetTime(AsyncWebServerRequest *request) {
        if(request->hasParam("timestamp")) { 
            startTimestamp = request->getParam("timestamp")->value().c_str();
            
            time_t now = (time_t)std::stoi(startTimestamp);
            struct timeval tv;
            tv.tv_sec = now;
            tv.tv_usec = 0;
            settimeofday(&tv, NULL);
            
            Serial.println("[TimestampManager] Updated local machine time from ClientSide!");
        }
        request->send(200, "text/plain", "OK");
    }

    void SetRoutes() {
        server.on("/get_time", HTTP_GET, handleGetTime);
    }
}