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

 //Global variables initialization and non-volatile variables handling

#ifndef GLOBALSTATEMANAGER_H
#define GLOBALSTATEMANAGER_H

#include <Arduino.h>
#include "datatypes.h"
#include <Wire.h>
#include <WiFi.h>
#include "BaseMCU.h"
#include "PAC194x.h"
#include "esp_system.h"
#include <Preferences.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "blobdata.h"

#define UIH_NAMESPACE "uih-nvm-1"
#define MEM_INITIALIZED_NUM 55
#define CONFIG_AUTO_SAVE_PERIOD 100

void globalStateInitializer(GlobalState *globalState, GlobalConfig *globalConfig);
void taskConfigAutoSave(void *pvParameters);
void saveMCUState(void);
void saveConfig(void);
void check_reset_reason();

#endif