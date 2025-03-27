/**
 *   USB Insight Hub
 *
 *   A USB supercharged interfacing tool for developers & tech enthusiasts wrapped 
 *   around ESP32 SvelteKit framework.
 *   https://github.com/Aeriosolutions/USB-Insight-HUB-Software
 *
 *   Copyright (C) 2024 - 2025 Aeriosolutions
 *   Copyright (C) 2024 - 2025 JoDaSa

 * MIT License. Check full description on LICENSE file.
 **/

 // task for the power startup logic, when enabled

#ifndef POWERSTARTUP_H
#define POWERSTARTUP_H

#include <Arduino.h>
#include "datatypes.h"

#define STARTUP_TIMER_PERIOD 100 //ms

void iniPowerStartUp(GlobalState* globalState, GlobalConfig* globalConfig);

#endif //POWERSTARTUP_H
