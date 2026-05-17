#include "../headers/Screen.h"

LiquidCrystal lcd(14, 13, 27, 23, 19, 21);

namespace Screen {
    std::vector<std::string> queue;
    unsigned long checkMillis = 0;
    std:: string screenText;
    std::string defaultText;

    void Init() 
    {
        lcd.begin(16, 2);
        Serial.println("[LCD] Initialized.");
    }

    void Print(std::string str)
    {
        lcd.clear();

        int totalChars = str.length();
        size_t newlinePos = str.find('\n');

        if (newlinePos != std::string::npos) 
        {
            std::string firstLine = str.substr(0, newlinePos);
            std::string secondLine = str.substr(newlinePos + 1);

            if (firstLine.length() > 16) firstLine = firstLine.substr(0, 16);
            if (secondLine.length() > 16) secondLine = secondLine.substr(0, 16);

            lcd.setCursor(0, 0);
            lcd.print(firstLine.c_str());

            if (!secondLine.empty()) {
                lcd.setCursor(0, 1);
                lcd.print(secondLine.c_str());
            }
        }
        else 
        {
            int rows = (totalChars == 0) ? 0 : (totalChars - 1) / 16;
            if (rows > 1) rows = 1; 

            for(int i = 0; i <= rows; i++)
            {
                std::string lineSegment = str.substr(i * 16, 16);
                lcd.setCursor(0, i);
                lcd.print(lineSegment.c_str()); 
            }
        }

        Serial.printf("\n[LCD Text]: %s\n", str.c_str());
        screenText = str;
    }

    void SetDefaultText(std::string str)
    {
        defaultText = str;
        if(queue.size() == 0)
            Print(defaultText);
    }

    void AddToQueue(std::string str)
    {
        queue.push_back(str);
        checkMillis = 0;
    }

    void UpdateDefaultText()
    {
        TaskManager::Task task = TaskManager::GetNextClosestTask();
        defaultText = basic_format("Uptime: {}\nUrm. Task: {}:{}.", transformaMilisecunde(UptimeManager::GetUptime()), task.hour, task.minute);
        SetDefaultText(defaultText);
    }

    void Loop()
    {
        if (millis() - checkMillis > TEXT_DURATION) {
            checkMillis = millis();

            if(queue.size() > 0)
            {
                if(queue[0] != screenText)
                    Print(queue[0]);
                
                queue.erase(queue.begin());
            }
            else
                SetDefaultText(defaultText);
       }
    }
}