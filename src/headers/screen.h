#pragma once
#include "LiquidCrystal_I2C.h"
#include <vector>
#include <headers/utils.h>
#include <headers/uptime.h>
#include "Wire.h"
#include "../headers/task_manager.h"

#define TEXT_DURATION    5*1000

namespace Screen {
    void Init();
    void Print(std:: string str);
    void Loop();
    void AddToQueue(std::string str);
    void SetDefaultText(std::string str);
    void UpdateDefaultText();
}