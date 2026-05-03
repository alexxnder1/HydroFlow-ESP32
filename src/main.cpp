#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// PINS
#define RELAY_GPIO   25

// Others
#define NOT_SET 67
#define TASK_DURATION 3000 

const char* ssid = "HydroFlow";
const char* password = "test";

unsigned long getDateCheckMillis = 0;
unsigned long relayMilis = 0;

AsyncWebServer server(80);
String timestamp = "";

void handleGetTime(AsyncWebServerRequest *request)
{
  if(request->hasParam("timestamp"))
  { 
    timestamp = request->getParam("timestamp")->value();
    Serial.println("Initialized!");
  }
  request->send(200, "text/plain", "OK");
}

bool relayActivated = false;

void ActivateElectrovalve()
{
  digitalWrite(RELAY_GPIO, HIGH);
  relayActivated = true;
  relayMilis = millis();
}
void handleForceTask(AsyncWebServerRequest *request)
{
  ActivateElectrovalve();
  request->send(200, "text/plain", "OK");
}

void DisableElectrovalve()
{
  digitalWrite(RELAY_GPIO, LOW);
  relayActivated = false;
}
struct Task {
  uint8_t hour;
  uint8_t minute;
  
  uint8_t day;
  uint8_t month;
  uint8_t year;
};

std::vector<Task> tasks;

void handleTasks(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
  String body = String((char*) data);
  
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, body);

  if (error) 
  {
    request->send(400, "application/json", "{\"error\":\"invalid json\"}");
    Serial.println("Cannot deserialize tasks json.");
    return;
  }
  
  JsonArray jsonTasks = doc.as<JsonArray>();
  tasks.clear();

  for(JsonObject obj : jsonTasks)
  {
    Task t;
    t.hour = obj["hour"];
    t.minute = obj["minute"];
    t.day = obj["day"];
    t.year = obj["year"];
    t.month = obj["month"];
  
    Serial.printf("\n--------\nTask #%d:\n * hour: %d, minute: %d, day: %d, year: %d, month: %d\n---------\n");
    
    tasks.push_back(t);
  }

  Serial.printf("\nLoaded %d tasks.\n", jsonTasks.size());
  request->send(200, "application/json", "{\"ok\":true}");
}

void setup() {
  
  Serial.begin(115200);
  
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed!");
    return;
  }
  Serial.println("LittleFS mounted!");

  WiFi.mode(WIFI_AP); 
  bool ok = WiFi.softAP("HydroFlow", "test1234");
  if (ok) {
    Serial.println("Access Point pornit!");
    Serial.print("IP: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("Eroare la pornirea AP!");
  }

  pinMode(RELAY_GPIO, OUTPUT);
  DisableElectrovalve();
  
  // Routes

  server.on("/get_time", HTTP_GET, handleGetTime);
  server.on("/force_task", HTTP_GET, handleForceTask);

  server.on("/get_tasks", HTTP_POST, [](AsyncWebServerRequest *request){},
    NULL,
    handleTasks
  );

  server.serveStatic("/", LittleFS, "/")
    .setDefaultFile("index.html");

  
  server.begin();
}
tm* GetDate()
{
  if(timestamp == "")
  { 
    Serial.println("Timestamp not set.");
    return nullptr;
  }

  time_t ts = atol(timestamp.c_str()) / 1000;

  static struct tm timeinfo;
  localtime_r(&ts, &timeinfo);

  return &timeinfo;
}


void loop() { 

  
  if (millis() - getDateCheckMillis > 1500) {
    getDateCheckMillis = millis();

    if(timestamp == "")  return; 
    if(tasks.size() == 0) return;

    tm* date = GetDate();
    if(!date) return;
    Serial.printf("\nGetDate returned: %d.%d.%d, %d:%d\n", date->tm_year, date->tm_mon, date->tm_mday, date->tm_hour, date->tm_min);

    for(Task &task : tasks) 
    {
      if(task.day == date->tm_mday && task.month == date->tm_mon && task.hour == date->tm_hour && task.minute == date->tm_min)
      {
        if(!relayActivated)
          ActivateElectrovalve();
      }
    }
  }

  if(millis() - relayMilis > TASK_DURATION) {
    DisableElectrovalve();
  }
}