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
#define BIN_ESCAPE            0x01  // SOH — binary frame escape byte
#define BIN_PROTOCOL_VERSION  0x01
#define BIN_HEADER_SIZE       10    // version(1) + cmd(2) + flags(2) + length(4) + checksum placeholder
#define BIN_CHECKSUM_SIZE     4
#define BIN_MAX_PAYLOAD       131072  // 128KB max payload

// Binary command types
#define BIN_CMD_IMAGE         0x0001
#define BIN_CMD_ECHO          0x0002
#define BIN_CMD_METER_STREAM  0x0003
#define BIN_CMD_SCREEN_LOCK   0x0004
#define BIN_CMD_SCREEN_READY  0x0005

// Pluggable binary command handler
struct BinCommandHandler {
    uint16_t cmd;
    bool (*begin)(uint16_t cmd, uint16_t flags, uint32_t payloadLen);
    void (*payloadByte)(uint8_t byte);
    void (*payloadBlock)(const uint8_t* data, size_t len);  // optional bulk handler
    void (*dispatch)();
    void (*reset)();   // optional, may be nullptr
};

#define BIN_MAX_HANDLERS 8

void binRegisterCommand(const BinCommandHandler* handler);

void iniExtercomms(GlobalState* globalState,GlobalConfig* globalConfig);

// Binary transport needs access to Screen for direct-to-display streaming
class Screen;
void iniExtercommsBinaryTransport(Screen* screen);

// Image write mode flags (selected via binary frame flags field)
#define IMG_FLAG_BUFFER  0   // buffer pixels → flush after CRC (default)
#define IMG_FLAG_SPRITE  1   // write into TFT_eSprite (8bpp only)
#define IMG_FLAG_DIRECT  2   // stream directly to SPI (no buffer, CRC-unsafe)
#define IMG_FLAG_RLE     0x04  // bit 2: payload is RLE-compressed (count+value pairs)

// Image mode flags — when set, render loop skips that channel
extern volatile bool imageMode[3];

// Screen lock timeout
#define SCREEN_LOCK_TIMEOUT_MS  10000

// Screen ready notification — render loop signals when a locked channel's slot arrives
extern volatile uint8_t screenReadyPending;
extern SemaphoreHandle_t screenReadySemaphore;
extern TaskHandle_t exterTaskHandle;

#endif