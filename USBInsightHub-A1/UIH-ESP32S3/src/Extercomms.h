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

 //Handlers for the communication with external devices through USB Serial

#ifndef EXTERCOMMS_H
#define EXTERCOMMS_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include "USB.h"
#include "datatypes.h"
#include <ArduinoJson.h>

#define PC_CONNECTION_TIMEOUT   2500
#define SERIAL_CHECK_PERIOD     50         
#define MAX_BUFFER_SIZE         512

#define ARR_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

void iniExtercomms(GlobalState* globalState,GlobalConfig* globalConfig);


#endif