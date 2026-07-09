#include "./headers/task_manager.h"

namespace TaskManager {   
    unsigned long getDateCheckMillis = 0;
    int lastMinuteEnabled = -1;

    // TODO: load din Flash la tasks
    std::vector<Task> tasks;
    TaskSettings settings = {15};

    void handleForceTask(AsyncWebServerRequest *request)
    {
        Electrovalve::ActivateElectrovalve();
        Serial.println("Handle force task");
        NotificationManager::SendToWS("HydroFlow Tasks", "Task pornit fortat!");
        Screen::AddToQueue("[Task Fortat]\nSucces!");
        request->send(200, "text/plain", "OK");
    }

    void LoadTasksFromMemory()
    {
        JsonDocument doc = GetJsonDocument("tasks");
        JsonArray array = doc.as<JsonArray>();

        tasks.clear();

        if(!array.isNull())
        {
            for(JsonVariant t : array)
            {
                JsonObject task = t.as<JsonObject>();

                if(!task.isNull())
                    tasks.push_back({task["hour"], task["minute"]});
            }
        }
        
        std::string str = basic_format("[Tasks Incarcate]\n{} tasks.", tasks.size());
        Screen::AddToQueue(str);
    }
  void LoadTaskSettingsFromMemory()
    {
        JsonDocument doc = GetJsonDocument("task_settings");
        JsonArray array = doc.as<JsonArray>();

        if(!array.isNull())
        {
            for(JsonVariant t : array)
            {
                JsonObject task = t.as<JsonObject>();

                if(!task.isNull())
                    settings.duration = task["duration"] || 15;
            }
        }
        
        std::string str = basic_format("[Tasks Settings]\n{} duration.", settings.duration);
        Screen::AddToQueue(str);
    }
    
    void handleTasks(AsyncWebServerRequest* request)
    {
        JsonDocument doc;
        JsonArray array = doc.to<JsonArray>();

        for(int i = 0; i < tasks.size(); i++)
        {
            JsonObject obj = array.add<JsonObject>();
            Task task = tasks[i];
            obj["hour"] = task.hour;
            obj["minute"] = task.minute;   
        }

        String output;
        serializeJson(doc, output);
        request->send(200, "application/json", output);
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
                SaveTasksToFlash();
                Screen::AddToQueue(basic_format("[Task Salvat]\nOra {}, min {}.", h, m));
                Serial.printf("\n[Add Task] Hour: %d, Minute: %d.", h, m);
                NotificationManager::SendToWS("HydroFlow Tasks", basic_format("A fost adaugat un task la ora {}, minutul {}.", h, m));
            }
            requestBody = "";
        }
    }
    void handleStopTask(AsyncWebServerRequest* request)
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
                    Electrovalve::DisableElectrovalve();
                    Serial.printf("\n[Stop Task] Hour: %d, Minute: %d", hour, minute);
                    Screen::AddToQueue(basic_format("[Tasks]\nTask {}:{} oprit!", hour, minute));
                    found=true;
                    break;
                }
            }

            if(!found)
                Serial.printf("[Stop Task] Cannot find Task: %d, %d", hour, minute);
        }

        request->send(200, "text/plain", "OK");
    }
    
    void handleGetTaskDuration(AsyncWebServerRequest* request)
    {
        Serial.printf("\n[Get Task Duration] Sent task duration %d.", settings.duration);
        request->send(200, "text/plain", std::to_string(settings.duration).c_str());
    }
    
    void handleSetTaskDuration(AsyncWebServerRequest* request)
    {
        if( request->hasParam("d"))
        {
            int d = request->getParam("d")->value().toInt();
            
            settings.duration = d;
            SaveTasksSettings();

            Screen::AddToQueue(basic_format("[Durata Task]\nSetata la {} min.", d));
            NotificationManager::SendToWS("HydroFlow Tasks", basic_format("Durata task-ului a fost setat la {} minute.", d));
        }

        Serial.printf("\n[Set Task Duration from Client] Sent task duration %d.", settings.duration);
        request->send(200, "text/plain", "OK");
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
                    SaveTasksToFlash();

                    Serial.printf("\n[Remove Task] Hour: %d, Minute: %d", hour, minute);
                    Screen::AddToQueue(basic_format("[Tasks]\nTask {}:{} sters.", hour, minute));
                    NotificationManager::SendToWS("HydroFlow Tasks", basic_format("Task-ul de la ora {}, minutul {} a fost sters!", hour, minute));
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
        server.on("/get_task_duration", HTTP_GET, handleGetTaskDuration);
        server.on("/set_task_duration", HTTP_GET, handleSetTaskDuration);

        server.on("/add_task", HTTP_POST | HTTP_OPTIONS, [](AsyncWebServerRequest* request) {
            if (request->method() == HTTP_OPTIONS) {
                AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "");
                response->addHeader("Access-Control-Allow-Origin", "*");
                response->addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
                response->addHeader("Access-Control-Allow-Headers", "Content-Type");
                request->send(response);
                return;
            }

            AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{\"status\":\"success\"}");
            response->addHeader("Access-Control-Allow-Origin", "*");
            request->send(response);
            
        }, NULL, handleAddTask);

        server.on("/delete_task", HTTP_GET, handleDeleteTask);
        server.on("/stop_task", HTTP_GET, handleStopTask);
    }

    tm GetDate()
    {
        time_t t;
        time(&t);
        struct tm timeinfo;
        localtime_r(&t, &timeinfo);
        return timeinfo;       
    }

    void CheckForTasks() {
        if (millis() - getDateCheckMillis > 5000) {
            getDateCheckMillis = millis();
            if(TimestampManager::startTimestamp == "") 
            {
                Serial.println("[TaskManager] Timestamp not set.");
                return; 
            }
            if(TaskManager::tasks.size() == 0) 
            {
                // Serial.println("[TaskManager] Task size is 0.");
                return; 
            }

            Screen::UpdateDefaultText();

            tm date = GetDate();

            if(!Electrovalve::enable && lastMinuteEnabled != date.tm_min) {

                for(TaskManager::Task &task : TaskManager::tasks) 
                {
                    // Serial.printf("\nTask returned: %d-%d, %d-%d\n", task.hour, date.tm_hour, task.minute, date.tm_min);
                    if(task.hour == date.tm_hour && task.minute == date.tm_min)
                    {
                        Electrovalve::ActivateElectrovalve();
                        NotificationManager::SendToWS("HydroFlow Tasks", basic_format("Task-ul de la ora {}, minutul {} a pornit pentru {} minute.", task.hour, task.minute, settings.duration));
                        lastMinuteEnabled = date.tm_min;
                        break;
                    }
                }
            }
        }

        if(Electrovalve::enable && millis() - Electrovalve::millisSinceStart > settings.duration*60*1000) {
            Electrovalve::DisableElectrovalve();
        }
    }
    TaskManager::Task GetNextClosestTask() 
    {
        if (tasks.empty()) 
            return {-1, -1}; 

        tm date = GetDate(); 
        int currentMinutesSinceMidnight = (date.tm_hour * 60) + date.tm_min;

        TaskManager::Task closestTask = tasks[0];
        int minDifference = 9999; 

        for (const auto& task : tasks) 
        {
            int taskMinutesSinceMidnight = (task.hour * 60) + task.minute;
            int difference = 0;

            if (taskMinutesSinceMidnight >= currentMinutesSinceMidnight) 
                difference = taskMinutesSinceMidnight - currentMinutesSinceMidnight;
            else 
                difference = (1440 - currentMinutesSinceMidnight) + taskMinutesSinceMidnight;

            if (difference < minDifference) 
            {
                minDifference = difference;
                closestTask = task;
            }
        }

        return closestTask;
    }
    
    void SaveTasksToFlash() {
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

    void SaveTasksSettings() {
        WriteJsonDoc<TaskSettings>(
            "task_settings",
            settings,
            [](const TaskSettings& t, JsonObject obj)
            {
                obj["duration"] = t.duration;
            }
        );
    }
}