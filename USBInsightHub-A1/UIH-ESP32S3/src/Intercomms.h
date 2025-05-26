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

/*Definitions of functions and variables of processes related to the I2C
communitcaion between the Main MCU, the BaseMCU and the Power Meter
*/

#ifndef INTERCOMMS_H
#define INTERCOMMS_H

#include <Arduino.h>
#include <Wire.h>
#include "esp_adc_cal.h"
#include "BaseMCU.h"
#include "PAC194x.h"
#include "datatypes.h"
#include "GlobalStateManager.h"

//pin definitions in datatypes.h

#define CLEAR_ALERT_RETRIES 3

#define INTERCOMMS_PERIOD 50 //ms

#define ADC_NUMSAMPLES 10
#define DIV5VRATIO 3.21 //22.1k|10.0k
#define I2CSPEED 400000

void iniIntercomms(GlobalState *globalState, GlobalConfig *globalConfig);

float read5Vrail();

#endif