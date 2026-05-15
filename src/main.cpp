#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "HydroFlow";
const char* password = "test";

AsyncWebServer server(80);

#include "./headers/task_manager.h"
#include "./headers/electrovalve.h"
#include "./headers/timestamp.h"
#include "./headers/json.h"
#include "./headers/uptime.h"

#define STA_MODE

void setup() {
  Serial.begin(115200);
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  // if (!LittleFS.begin(true)) {
  //   Serial.println("LittleFS mount failed!");
  //   return;
  // }
  // Serial.println("LittleFS mounted!");

  #ifdef STA_MODE
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    WiFi.begin("0x9988b7", "ghitaeprost");
    Serial.printf("Connecting to WiFi....");
    Serial.println(WiFi.getTxPower()); // Returns value in 0.25dBm units

    Serial.printf("\nConnecting to Hotspot");
    while(WiFi.status() != WL_CONNECTED)
    { 
      delay(50);
      Serial.print("."); 
    }

    Serial.printf("\nSTA IP Address: %s\n", WiFi.localIP().toString().c_str());

  #else
  
    WiFi.mode(WIFI_AP); 
    bool ok = WiFi.softAP("HydroFlow", "test1234");
    Serial.println(WiFi.getTxPower());
    if (ok) {
      Serial.println("Access Point pornit!");
      Serial.print("IP: ");
      Serial.println(WiFi.softAPIP());
    } else {
      Serial.println("Eroare la pornirea AP!");
    }

  #endif

  TaskManager::LoadTasksFromMemory();

  Electrovalve::SetGPIOToOutput();
  Electrovalve::DisableElectrovalve();
  
  // Routes
  TimestampManager::SetRoutes();
  TaskManager::SetRoutes();
  UptimeManager::SetRoutes();

  // server.serveStatic("/", LittleFS, "/")
  //   .setDefaultFile("index.html");
  
  server.begin();
}

void loop() {  
  TaskManager::CheckForTasks();
  delay(10);
} 