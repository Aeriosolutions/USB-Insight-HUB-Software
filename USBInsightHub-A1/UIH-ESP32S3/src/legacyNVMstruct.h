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

 //this library defines the structure of the NVM parameters used in previous versions of the firmware, in case we need to read them for migration purposes.

#ifndef LEGACYNVMSTRUCT_H
#define LEGACYNVMSTRUCT_H

#include <Arduino.h>
#include <Wire.h>
#include "PAC194x.h"
#include "Screen.h"

#define LEGACY_DATATYPES_VER 4 // this is the template in version 1.0.0, the one all the first and second batch were flashed to

struct lFeaturesConfig {
  uint8_t startView; 
  uint8_t startUpmode;  
  uint8_t wifi_enabled;
  uint8_t hubMode;
  uint8_t filterType;    
  uint8_t refreshRate;
};

struct lStartupConfig {
  int startup_timer;
};

struct lScreenConfig {
  uint8_t rotation;
  uint16_t brightness;
};

struct lMeterConfig {
  uint16_t fwdCLim;
  uint16_t backCLim;
};

struct lGlobalConfig {
    lFeaturesConfig features;
    lStartupConfig startup[3];
    lScreenConfig screen[3];
    lMeterConfig meter[3];
    //BaseMCUConfig baseMCU[3];
};


#endif // LEGACYNVMSTRUCT_H