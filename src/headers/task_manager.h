#pragma once
#include <vector>
#include <ESPAsyncWebServer.h>
#include "./headers/electrovalve.h"
#include "./headers/timestamp.h"
#include "./headers/json.h"
#include "LittleFS.h"

// #define TASK_DURATION 15*60*1000 
#define TASK_DURATION 2000

namespace TaskManager {
    extern unsigned long getDateCheckMillis;
    extern int lastMinuteEnabled;

    class Task {
        public: 
        int hour;
        int minute;
        Task(int h, int m) : hour(h), minute(m) {}
    };
    
    void handleForceTask(AsyncWebServerRequest *request);

    void handleTasks(AsyncWebServerRequest* request);
    void SetRoutes();

    // Default Tasks
    extern std::vector<Task> tasks;

    tm GetDate();
    void CheckForTasks();
    void LoadTasksFromMemory();
    void SaveTasksToFlash();
}