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
#define MAX_BUFFER_SIZE         1024
#define DISPLAY_CLEAR_AFTER_TIMEOUT  2000

#define ARR_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

// Binary transport protocol
#define BIN_PROTOCOL_VERSION  0x01
#define BIN_HEADER_SIZE       10    // version(1) + cmd(2) + flags(2) + length(4) + checksum placeholder
#define BIN_CHECKSUM_SIZE     4
#define BIN_MAX_PAYLOAD       131072  // 128KB max payload

// Binary command types
#define BIN_CMD_IMAGE         0x0001
#define BIN_CMD_ECHO          0x0002

void iniExtercomms(GlobalState* globalState,GlobalConfig* globalConfig);

// Binary transport needs access to Screen for direct-to-display streaming
class Screen;
void iniExtercommsBinaryTransport(Screen* screen);

// Image mode flags — when set, render loop skips that channel
extern volatile bool imageMode[3];

#endif