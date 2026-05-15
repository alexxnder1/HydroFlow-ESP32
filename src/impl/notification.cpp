#include "./headers/notification.h"

namespace NotificationManager {
    void SendToWS(std:: string title, std::string body) {
        JsonDocument doc;
        doc["title"] = title;
        doc["body"] = body;

        String output;
        serializeJson(doc, output);
        String finalMessage = "[NOTIFICATION]" + output;
        WebSocket::ws.textAll(finalMessage);
    }
}