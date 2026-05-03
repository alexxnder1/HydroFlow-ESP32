#include <string>
#include "./headers/timestamp.h"
#include <ESPAsyncWebServer.h>

namespace TimestampManager {
    std::string timestamp = "";

    void handleGetTime(AsyncWebServerRequest *request) {
        if(request->hasParam("timestamp")) { 
            timestamp = request->getParam("timestamp")->value().c_str();
            Serial.println("Timestamp actualizat!");
        }
        request->send(200, "text/plain", "OK");
    }

    void SetRoutes() {
        server.on("/get_time", HTTP_GET, handleGetTime);
    }
}