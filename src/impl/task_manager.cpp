#include "./headers/task_manager.h"

namespace TaskManager {   
    unsigned long getDateCheckMillis = 0;
    int lastMinuteEnabled = -1;
    // TODO: load din Flash la tasks
    std::vector<Task> tasks = {
        {4, 15},
        {9, 0},
        {21, 30}
    };

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

    void SetRoutes() {
        server.on("/force_task", HTTP_GET, handleForceTask);
        server.on("/get_tasks", HTTP_GET, handleTasks);
    }

    tm* GetDate()
    {
        time_t t;
        struct tm *info;

        time(&t);
        return localtime(&t);

        
        // if(TimestampManager::startTimestamp == "")
        // { 
        //     Serial.  ("Timestamp not set.");
        //     return nullptr;
        // }

        // time_t ts = atol(TimestampManager::startTimestamp.c_str()) / 1000;

        // static struct tm timeinfo;
        // localtime_r(&ts, &timeinfo);
        // return &timeinfo;
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