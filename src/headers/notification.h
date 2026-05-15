#pragma once
#include <string>
#include "ArduinoJson.h"
#include "web_socket.h"

namespace NotificationManager {
    void SendToWS(std:: string title, std::string body);
}