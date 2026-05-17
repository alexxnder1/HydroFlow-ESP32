#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "HydroFlow";
const char* password = "test";

AsyncWebServer server(80);
AsyncWebSocket ws("ws://192.168.0.142:80");

#include "./headers/task_manager.h"
#include "./headers/electrovalve.h"
#include "./headers/timestamp.h"
#include "./headers/json.h"
#include "./headers/uptime.h"
#include "./headers/web_socket.h"
#include "./headers/screen.h"
#include "./headers/utils.h"

// this module is not published on github 
#include "./privacy.h"

#define STA_MODE

void setup() {
  Serial.begin(115200);
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed!");
    return;
  }
  Serial.println("LittleFS mounted!");

  Screen::Init();

  #ifdef STA_MODE
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.begin(NETWORK_HOST, NETWORK_PASSWORD);

    Serial.printf("Connecting to WiFi....");
    Screen::AddToQueue(basic_format("Connecting to {}.", NETWORK_HOST));

    while(WiFi.status() != WL_CONNECTED)
    { 
      delay(50);
      Serial.print("."); 
    }

    Serial.printf("\nSTA IP Address: %s\n", WiFi.localIP().toString().c_str());
    Screen::AddToQueue(basic_format("Connected!\nQuality: {}", getSignalQualityText(WiFi.RSSI())));

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

  WebSocket::Setup();
}

void loop() {  
  TaskManager::CheckForTasks();
  Screen::Loop();
  delay(10);
} 