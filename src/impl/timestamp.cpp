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

            long long ms = std::stoll(startTimestamp);

            time_t now = (time_t)(ms / 1000);

            struct timeval tv;
            tv.tv_sec = now;
            tv.tv_usec = 0;

            settimeofday(&tv, NULL);    
            setenv("TZ", "EET-2EEST,M3.5.0/3,M10.5.0/4", 1);
            tzset();
            
            Serial.println("[TimestampManager] Updated local machine time from ClientSide!");
        }
        request->send(200, "text/plain", "OK");
    }

    void SetRoutes() {
        server.on("/get_time", HTTP_GET, handleGetTime);
    }
}