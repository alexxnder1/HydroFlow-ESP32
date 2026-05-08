#include "./headers/task_manager.h"

namespace TaskManager {   
    unsigned long getDateCheckMillis = 0;
    int lastMinuteEnabled = -1;
    // TODO: load din Flash la tasks
    std::vector<Task> tasks;

    void handleForceTask(AsyncWebServerRequest *request)
    {
        Electrovalve::ActivateElectrovalve();
        Serial.println("Handle force task");

        request->send(200, "text/plain", "OK");
    }

    void handleTasks(AsyncWebServerRequest* request)
    {
        if(LittleFS.exists("/tasks.json"))
            request->send(LittleFS, "/tasks.json", "application/json");

        else request->send(400, "application/json", "{\"error\":\"File not found\"}");
    }
    
    void handleAddTask(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) 
    {
        String requestBody="";
        
        for(size_t i = 0; i < len; i++)
            requestBody+=(char)data[i];

        if(index + len == total)
        {
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, requestBody);
            if(error) 
                Serial.printf("[Add Task Deserialization] Error Parsing Json: %s", error.c_str());
            else {
                int h = doc["hour"] | -1;
                int m = doc["minute"] | -1;

                if(h == -1 || m == -1)
                    Serial.printf("\n Error parsing task data.");

                tasks.push_back({h, m});
                WriteTasksToFlash();
                Serial.printf("\n[Add Task] Hour: %d, Minute: %d.", h, m);
            }
            requestBody = "";
        }
    }
    void handleDeleteTask(AsyncWebServerRequest* request)
    {
        if(request->hasParam("hour") && request->hasParam("minute"))
        {
            int hour = request->getParam("hour")->value().toInt();
            int minute = request->getParam("minute")->value().toInt();

            bool found=false;
            for(int i = 0; i < tasks.size(); i++)
            {
                if(tasks[i].hour == hour && tasks[i].minute == minute)
                {
                    tasks.erase(tasks.begin() + i);
                    WriteTasksToFlash();
                    Serial.printf("\n[Remove Task] Hour: %d, Minute: %d", hour, minute);
                    found=true;
                    break;
                }
            }
            if(!found)
                Serial.printf("[Delete Task] Cannot find Task: %d, %d", hour, minute);
        }

        request->send(200, "text/plain", "OK");
    }

    void SetRoutes() {
        server.on("/force_task", HTTP_GET, handleForceTask);
        server.on("/get_tasks", HTTP_GET, handleTasks);
        
        server.on("/add_task", HTTP_POST, [](AsyncWebServerRequest* request) {
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        }, NULL, handleAddTask);

        server.on("/delete_task", HTTP_GET, handleDeleteTask);
    }

    tm* GetDate()
    {
        time_t t;
        struct tm *info;

        time(&t);
        return localtime(&t);       
    }

    void CheckForTasks() {
        if (millis() - getDateCheckMillis > 1500) {
            getDateCheckMillis = millis();
            
            Serial.println(TimestampManager::startTimestamp.c_str());
            Serial.println(TaskManager::tasks.size());
            if(TimestampManager::startTimestamp == "") 
            {
                Serial.println("[TaskManager] Waiting for Client to retrieve timestamp...");
                return; 
            }
            if(TaskManager::tasks.size() == 0) 
            {
                Serial.println("[TaskManager] Task size is 0.");
                return; 
            }

            tm* date = GetDate();
            if(!date) return;
            // Serial.printf("\nGetDate returned: %d.%d.%d, %d:%d\n", date->tm_year, date->tm_mon, date->tm_mday, date->tm_hour, date->tm_min);

            if(!Electrovalve::enable && lastMinuteEnabled != date->tm_min) {
                for(TaskManager::Task &task : TaskManager::tasks) 
                {
                    if(task.hour == date->tm_hour && task.minute == date->tm_min)
                    {
                        Electrovalve::ActivateElectrovalve();
                        lastMinuteEnabled = date->tm_min;
                        break;
                    }
                }
            }
        }

        if(Electrovalve::enable && millis() - Electrovalve::millisSinceStart > TASK_DURATION) {
            Electrovalve::DisableElectrovalve();
        }
    }

    void WriteTasksToFlash() {
        WriteJsonDoc<Task>(
            "tasks",
            tasks,
            [](const Task& t, JsonObject obj)
            {
                obj["hour"] = t.hour;
                obj["minute"] = t.minute;
            }
        );

        Serial.printf("[TaskManager] Overwriting the default tasks to Flash.");
    }
}