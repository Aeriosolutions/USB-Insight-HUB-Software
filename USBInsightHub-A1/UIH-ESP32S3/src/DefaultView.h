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

//Logic for the default view (Devices metadata, voltage/current meter, etc.)

#ifndef DEFAULTVIEW_H
#define DEFAULTVIEW_H

#include <Arduino.h>
#include "Buttons.h"
#include "datatypes.h"
#include "Screen.h"
#include "MenuView.h"

#define DEFAULT_VIEW_PERIOD      40 //in ms
#define MENU_INFO_SPLASH_TIMEOUT 2000 //in ms



void iniDefaultView(GlobalState* globalState, GlobalConfig* globalConfig, Screen *screen);
void defaultViewStart(void);

extern SemaphoreHandle_t screen_Semaphore;

#endif