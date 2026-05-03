#pragma once
#include <Arduino.h>

// PINS
#define RELAY_GPIO   25

namespace Electrovalve {
    extern bool enable;
    extern unsigned long millisSinceStart;

    void SetGPIOToOutput();
    void ActivateElectrovalve();    
    void DisableElectrovalve();
}