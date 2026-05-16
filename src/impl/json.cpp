#include "./headers/json.h"

JsonDocument GetJsonDocument(const char* fileName) {

    char path[64];
    snprintf(path, sizeof(path), "/%s.json",  fileName);
    File file = LittleFS.open(path, "r");
    if (!file) {
        Serial.printf("\n[SaveConfig] Error opening file %s!\n",  fileName);
    }
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);

    if(error)
    {
        Serial.printf("\n[SaveConfig]   Error deserializing JSON for file %s, %s\n",  fileName, error.c_str());
    }   

    file.close();
    
    return doc;
}
