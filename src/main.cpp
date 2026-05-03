#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define DEBUG

const char* ssid = "HydroFlow";
const char* password = "test";

AsyncWebServer server(80);

#include "./headers/task_manager.h"
#include "./headers/electrovalve.h"
#include "./headers/timestamp.h"
#include "./headers/json.h"

void setup() {
  
  Serial.begin(115200);
  Serial.println("test");
  
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed!");
    return;
  }
  Serial.println("LittleFS mounted!");

  TaskManager::WriteTasksToFlash();  

  #ifdef DEBUG
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    WiFi.begin("0x9988b7", "ghitaeprost");
    printf("Connecting to WiFi....");

    Serial.printf("\nConnecting to Hotspot");
    while(WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print("."); 
    }

    Serial.printf("\nSTA IP Address: %s\n", WiFi.localIP().toString().c_str());

  #else
  
    WiFi.mode(WIFI_AP); 
    bool ok = WiFi.softAP("HydroFlow", "test1234");
    
    if (ok) {
      Serial.println("Access Point pornit!");
      Serial.print("IP: ");
      Serial.println(WiFi.softAPIP());
    } else {
      Serial.println("Eroare la pornirea AP!");
    }

  #endif

  Electrovalve::SetGPIOToOutput();
  Electrovalve::DisableElectrovalve();
  
  // Routes
  TimestampManager::SetRoutes();
  TaskManager::SetRoutes();

  server.serveStatic("/", LittleFS, "/")
    .setDefaultFile("index.html");
  
  server.begin();
}

void loop() {  
  TaskManager::CheckForTasks();
}