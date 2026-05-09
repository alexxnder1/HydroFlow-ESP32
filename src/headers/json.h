#pragma once
#include "config.h"
#include <vector>
#include <string>
#include <ArduinoJson.h>
#include "LittleFS.h"

JsonDocument GetJsonDocument(const char* fileName);

template <typename T, typename F>
void WriteJsonDoc(const char* fileName, const std::vector<T>& data, F toJson) {
    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();
    
    for (const T& item : data) {
        JsonObject obj = arr.add<JsonObject>(); 
        toJson(item, obj);
    }

    char path[64];
    snprintf(path, sizeof(path), "/%s.json", fileName);
    File file = LittleFS.open(path, "w");
    if (!file) {
        Serial.printf("\n[SaveConfig] Error opening file %s!\n", fileName);
        return;
    }

    if(serializeJson(doc, file) == 0) {
        Serial.printf("\n[SaveConfig] Error serializing JSON for file %s\n", fileName);
    }
    file.close();
}