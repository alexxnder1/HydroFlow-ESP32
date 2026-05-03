#include "../headers/electrovalve.h"

// PINS
#define RELAY_GPIO   25

namespace Electrovalve {
    bool enable = false;
    unsigned long millisSinceStart = 0;

    void SetGPIOToOutput() {
        pinMode(RELAY_GPIO, OUTPUT);
    }

    void ActivateElectrovalve()
    {
        digitalWrite(RELAY_GPIO, HIGH);
        enable = true;
        millisSinceStart = millis();
    }

    void DisableElectrovalve()
    {
        digitalWrite(RELAY_GPIO, LOW);
        enable = false;
    }
}