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

#include "Extercomms.h"
#include "tusb.h"
#include "DefaultView.h"
#include "Screen.h"
#include <RestartService.h>
#include <esp_crc.h>

//USB Serial and Harware Serial (Debug)
#if ARDUINO_USB_CDC_ON_BOOT
#define HWSerial Serial0
#define USBSerial Serial
#else
#define HWSerial Serial
USBCDC usbSerial;
#endif

static const char* TAG = "Extercoms";

#define IMAGE_SIZE_PIXELS (226*90) //Width*Height*1bpp
#define SERIAL_BUFFER_TIMEOUT_MS 1000

GlobalState *gloState;
GlobalConfig *gloConfig;
static Screen *gloScreen = nullptr;

bool USBSerialActivity = false;
bool dataReceived = false;

volatile bool imageMode[3] = {false, false, false};

char rawBuffer[80];
size_t rawBufIndex = 0;
char inputBuffer[MAX_BUFFER_SIZE];   //working array JSON-RPC
size_t bufferIndex = 0;
int8_t imgPortIndex = -1;
uint8_t imgBufBpp = 0;
size_t imgBufLen = 0;
unsigned long long lastSerialTime = 0;

// Binary transport state machine
enum ParseState {
  PARSE_TEXT,
  PARSE_BIN_HEADER,
  PARSE_BIN_PAYLOAD,
  PARSE_BIN_CHECKSUM
};

static ParseState parseState = PARSE_TEXT;
static uint8_t binHeader[10];   // version(1) + cmd(2) + flags(2) + length(4) = 9 bytes, + 1 spare
static size_t binHeaderIndex = 0;
static uint32_t binPayloadLen = 0;
static uint32_t binPayloadReceived = 0;
static uint16_t binCmd = 0;
static uint16_t binFlags = 0;
static uint32_t binRunningCRC = 0;
static uint8_t binChecksumBuf[4];
static size_t binChecksumIndex = 0;

// Image sub-header state
static const size_t IMAGE_SUBHEADER_SIZE = 6;  // port(1) + bpp(1) + width(2) + height(2)
static uint8_t imgSubHeader[6];
static size_t imgSubHeaderIndex = 0;
static bool imgStreamActive = false;
static uint8_t imgPort = 0;
static uint8_t imgBpp = 0;
static uint16_t imgWidth = 0;
static uint16_t imgHeight = 0;
static uint32_t imgPixelsExpected = 0;
static uint32_t imgBytesReceived = 0;
static bool imgFailed = false;  // set if image validation failed; suppresses dispatch

// Small pixel buffer for streaming to SPI
static uint16_t pixStreamBuf[64];
static uint8_t pixStreamRaw[128]; // for accumulating partial pixel bytes at 16bpp
static size_t pixStreamBufCount = 0;
static size_t pixStreamRawCount = 0;

// CRC-32 using ESP32 ROM hardware acceleration (esp_crc32_le)
// No lookup table needed — uses the chip's built-in CRC unit.

// Echo command state
static uint8_t* echoBuffer = nullptr;
static size_t echoBufferSize = 0;
static size_t echoBufferIndex = 0;

// Forward declarations
static void binReset();
static void binDispatch();
static void binProcessPayloadByte(uint8_t byte);
static void binImageBegin();
static void binImagePayloadByte(uint8_t byte);
static void binImageFlushPixels();
static void binImageEnd(bool success);
static void binEchoBegin();
static void binEchoPayloadByte(uint8_t byte);
static void binEchoEnd();
static void binSendResponse(uint16_t cmd, const char* message, bool ok = true);

//Internal functions
void parseDataPC();
static void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
void taskExterCheckActivity(void *pvParameters);

void onSerialDataReceived();
void processJsonRpcMessage(const char* jsonString);
void sendJsonResponse(int id, JsonVariant result);
void printErr(String err);
int getEnumIndex(const char* name, const char* const* array, int size);


// --- USB bootloader entry via 1200-baud touch ---
// The Arduino core's usb_persist_restart() uses esp_restart() which only resets
// CPUs on ESP32-S3, not digital peripherals. The USB-OTG device stays alive and
// the ROM never enters download mode. We intercept via --wrap and use a deferred
// FreeRTOS task with a true system reset (RTC_CNTL_SW_SYS_RST).
#include "soc/rtc_cntl_reg.h"

extern "C" void __real_usb_persist_restart(uint32_t mode);
extern "C" void usb_persist_restart(uint32_t mode);

// Deferred bootloader entry — runs outside the USB callback context.
// The 1200-baud detection fires inside a TinyUSB SET_LINE_CODING callback;
// we can't reset from there, so we defer to a FreeRTOS task.
static void bootloaderTask(void* param) {
    delay(100);  // let USB callback return

    // On ESP32-S3, esp_restart() only resets CPUs, NOT digital peripherals.
    // We need a true system reset so the ROM re-initializes USB Serial JTAG
    // and checks FORCE_DOWNLOAD_BOOT.

    // 1. Disconnect TinyUSB cleanly so host processes removal
    tud_disconnect();
    delay(500);

    // 2. Route PHY to USB Serial JTAG (clear SW override → hardware default)
    CLEAR_PERI_REG_MASK(RTC_CNTL_USB_CONF_REG,
        RTC_CNTL_SW_HW_USB_PHY_SEL | RTC_CNTL_SW_USB_PHY_SEL |
        RTC_CNTL_USB_PAD_ENABLE);

    // 3. Set FORCE_DOWNLOAD_BOOT
    REG_WRITE(RTC_CNTL_OPTION1_REG, RTC_CNTL_FORCE_DOWNLOAD_BOOT);

    // 4. True system reset (resets CPU + all digital peripherals).
    //    esp_restart()/esp_restart_noos() only reset CPUs on ESP32-S3.
    SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_SW_SYS_RST);
    while (true) { ; }
}

extern "C" void __wrap_usb_persist_restart(uint32_t mode) {
    if (mode == 2) {  // RESTART_BOOTLOADER
        // Defer to a task so we exit the USB callback context first
        xTaskCreate(bootloaderTask, "bootloader", 4096, NULL, configMAX_PRIORITIES - 1, NULL);
        return;  // return to USB callback — task will handle the rest
    }
    __real_usb_persist_restart(mode);
}

void iniExtercomms(GlobalState* globalState, GlobalConfig* globalConfig){

    gloState = globalState;
    gloConfig = globalConfig;

    // Restore USB-OTG PHY selection. After bootloader entry, we clear these bits
    // to route the PHY to USB Serial JTAG. The RTC register persists across resets
    // and even short power cycles. Without this restore, USB-OTG won't work after
    // a bootloader→app transition until the RTC domain drains (~30s).
    SET_PERI_REG_MASK(RTC_CNTL_USB_CONF_REG,
        RTC_CNTL_SW_HW_USB_PHY_SEL | RTC_CNTL_SW_USB_PHY_SEL | RTC_CNTL_USB_PAD_ENABLE);

    //Hardware Serial Ini
    //HWSerial.begin(115200); //Debug Serial
    USB.onEvent(usbEventCallback);
    usbSerial.onEvent(usbEventCallback);

    usbSerial.begin(115200);
    usbSerial.enableReboot(globalConfig->features.reboot_enabled == ENABLE);
    USB.manufacturerName("Aerio");
    USB.productName("InsightHUB Controller");
    USB.begin();

    xTaskCreatePinnedToCore(taskExterCheckActivity, "Extercom check", 4096, NULL, 5, NULL, APP_CORE);

}

void iniExtercommsBinaryTransport(Screen* screen){
  gloScreen = screen;
}

//serial loop - check if there has been serial activity to update the pc-connection status icon 
void taskExterCheckActivity(void *pvParameters){
    TickType_t xLastWakeTime = xTaskGetTickCount();
    unsigned long lastPCcom;
    unsigned long now;
    for(;;){
        now= millis();

        if(USBSerialActivity){
          lastPCcom = millis();
          gloState->features.pcConnected = true;
          gloState->features.clearScreenText = false;
          USBSerialActivity=false;
        }

        if(now-lastPCcom > PC_CONNECTION_TIMEOUT){
          gloState->features.pcConnected = false;
        }

        if(now-lastPCcom > PC_CONNECTION_TIMEOUT+DISPLAY_CLEAR_AFTER_TIMEOUT 
           && gloState->features.enableClearScreenText){
          //clear display texts.
          gloState->features.clearScreenText = true;    
          for(int i=0; i<3; i++){
            gloState->usbInfo[i].numDev = 0; //avoids reprint of previous texts
            gloState->usbInfo[i].usbType = 0; //avoids reprint of previous texts
          }
        }

        if(dataReceived){          
          processJsonRpcMessage(inputBuffer);  // Process JSON          
          dataReceived = false;
        }

        vTaskDelayUntil(&xLastWakeTime,pdMS_TO_TICKS(SERIAL_CHECK_PERIOD));
        //vTaskDelay(pdMS_TO_TICKS(SERIAL_CHECK_PERIOD));
    }

}

static void binReset() {
  parseState = PARSE_TEXT;
  binHeaderIndex = 0;
  binPayloadLen = 0;
  binPayloadReceived = 0;
  binCmd = 0;
  binFlags = 0;
  binRunningCRC = 0;
  binChecksumIndex = 0;
  imgSubHeaderIndex = 0;
  imgStreamActive = false;
  imgFailed = false;
  imgBytesReceived = 0;
  pixStreamBufCount = 0;
  pixStreamRawCount = 0;
  if (echoBuffer) { free(echoBuffer); echoBuffer = nullptr; }
  echoBufferSize = 0;
  echoBufferIndex = 0;
}

static void binSendResponse(uint16_t cmd, const char* message, bool ok) {
  char buf[128];
  snprintf(buf, sizeof(buf),
    "{\"status\":\"%s\",\"data\":{\"cmd\":%u,\"message\":\"%s\"}}",
    ok ? "ok" : "error", cmd, message);
  usbSerial.println(buf);
  usbSerial.flush();
}

// Called when full binary frame is received and checksum verified
static void binDispatch() {
  switch (binCmd) {
    case BIN_CMD_IMAGE:
      if (!imgFailed) {
        binImageEnd(true);
      }
      // If imgFailed, error was already sent during validation
      break;
    case BIN_CMD_ECHO:
      binEchoEnd();
      break;
    default:
      binSendResponse(binCmd, "unknown command", false);
      break;
  }
}

// Process image sub-header bytes, then stream pixel data
static void binImagePayloadByte(uint8_t byte) {
  if (imgFailed) return;  // validation failed — consume silently

  if (imgSubHeaderIndex < IMAGE_SUBHEADER_SIZE) {
    imgSubHeader[imgSubHeaderIndex++] = byte;
    if (imgSubHeaderIndex == IMAGE_SUBHEADER_SIZE) {
      binImageBegin();
    }
    return;
  }

  if (imgFailed) return;  // binImageBegin() may have set this

  // Pixel data — stream to display
  imgBytesReceived++;

  if (imgBpp == 16) {
    // Accumulate pairs of bytes into RGB565 pixels
    pixStreamRaw[pixStreamRawCount++] = byte;
    if (pixStreamRawCount >= 2) {
      // Little-endian RGB565
      pixStreamBuf[pixStreamBufCount++] = pixStreamRaw[0] | (pixStreamRaw[1] << 8);
      pixStreamRawCount = 0;
      if (pixStreamBufCount >= 64) {
        binImageFlushPixels();
      }
    }
  } else if (imgBpp == 8) {
    // RGB332 — convert via palette and buffer
    if (gloScreen) {
      pixStreamBuf[pixStreamBufCount++] = gloScreen->palette[byte];
    }
    if (pixStreamBufCount >= 64) {
      binImageFlushPixels();
    }
  }
}

static void binImageBegin() {
  imgPort   = imgSubHeader[0];
  imgBpp    = imgSubHeader[1];
  imgWidth  = imgSubHeader[2] | (imgSubHeader[3] << 8);
  imgHeight = imgSubHeader[4] | (imgSubHeader[5] << 8);

  uint32_t bytesPerPixel = (imgBpp + 7) / 8;
  imgPixelsExpected = (uint32_t)imgWidth * imgHeight;
  uint32_t expectedDataBytes = imgPixelsExpected * bytesPerPixel;
  uint32_t expectedPayload = IMAGE_SUBHEADER_SIZE + expectedDataBytes;

  // Validate — on failure, set imgFailed so remaining bytes are consumed silently
  if (imgPort < 1 || imgPort > 3) {
    ESP_LOGW(TAG, "Image: invalid port %u", imgPort);
    binSendResponse(BIN_CMD_IMAGE, "invalid port", false);
    imgFailed = true;
    return;
  }
  if (imgBpp != 8 && imgBpp != 16) {
    ESP_LOGW(TAG, "Image: unsupported bpp %u", imgBpp);
    binSendResponse(BIN_CMD_IMAGE, "unsupported bpp", false);
    imgFailed = true;
    return;
  }
  if (imgWidth == 0 || imgHeight == 0 || imgWidth > 240 || imgHeight > 240) {
    ESP_LOGW(TAG, "Image: invalid dimensions %ux%u", imgWidth, imgHeight);
    binSendResponse(BIN_CMD_IMAGE, "invalid dimensions", false);
    imgFailed = true;
    return;
  }
  if (expectedPayload != binPayloadLen) {
    ESP_LOGW(TAG, "Image: payload length mismatch (expected %u, got %u)", expectedPayload, binPayloadLen);
    binSendResponse(BIN_CMD_IMAGE, "payload length mismatch", false);
    imgFailed = true;
    return;
  }

  if (!gloScreen) {
    ESP_LOGE(TAG, "Image: screen not initialized");
    binSendResponse(BIN_CMD_IMAGE, "screen not initialized", false);
    imgFailed = true;
    return;
  }

  // Acquire screen semaphore — block up to 200ms
  if (xSemaphoreTake(screen_Semaphore, pdMS_TO_TICKS(200)) != pdTRUE) {
    ESP_LOGW(TAG, "Image: screen busy");
    binSendResponse(BIN_CMD_IMAGE, "screen busy", false);
    imgFailed = true;
    return;
  }

  // Mark this channel as image mode so render loop skips it
  imageMode[imgPort - 1] = true;

  // Determine display position — center the image in the info area
  int32_t x = 7;   // info area X offset
  int32_t y = 40;   // info area Y offset

  // Get CS pin for target display
  uint8_t cs_pin;
  switch (imgPort) {
    case 1: cs_pin = DISPLAY_CS_1; break;
    case 2: cs_pin = DISPLAY_CS_2; break;
    case 3: cs_pin = DISPLAY_CS_3; break;
    default: cs_pin = DISPLAY_CS_1; break;
  }

  gloScreen->streamBegin(cs_pin, x, y, imgWidth, imgHeight);
  imgStreamActive = true;
  imgBytesReceived = 0;
  pixStreamBufCount = 0;
  pixStreamRawCount = 0;

  ESP_LOGI(TAG, "Image: port=%u bpp=%u %ux%u", imgPort, imgBpp, imgWidth, imgHeight);
}

static void binImageFlushPixels() {
  if (pixStreamBufCount > 0 && gloScreen && imgStreamActive) {
    gloScreen->streamPixels(pixStreamBuf, pixStreamBufCount);
    pixStreamBufCount = 0;
  }
}

static void binImageEnd(bool success) {
  if (imgStreamActive) {
    // Flush any remaining pixels
    binImageFlushPixels();
    gloScreen->streamEnd();
    xSemaphoreGive(screen_Semaphore);
    imgStreamActive = false;
  }

  if (success) {
    binSendResponse(BIN_CMD_IMAGE, "image complete");
  }
  // Error responses are sent by the caller before calling binImageEnd(false)
}

// --- Echo command (cmd=0x0002) ---
// Echoes payload back as a binary frame with the same cmd.
// Useful for round-trip latency testing and protocol verification.

static void binEchoBegin() {
  if (binPayloadLen > 4096) {
    binSendResponse(BIN_CMD_ECHO, "echo payload too large", false);
    echoBuffer = nullptr;
    return;
  }
  echoBuffer = (uint8_t*)malloc(binPayloadLen);
  if (!echoBuffer) {
    binSendResponse(BIN_CMD_ECHO, "out of memory", false);
    return;
  }
  echoBufferSize = binPayloadLen;
  echoBufferIndex = 0;
}

static void binEchoPayloadByte(uint8_t byte) {
  if (echoBufferIndex == 0 && !echoBuffer) {
    binEchoBegin();
    if (!echoBuffer) return;  // allocation failed
  }
  if (echoBufferIndex < echoBufferSize) {
    echoBuffer[echoBufferIndex++] = byte;
  }
}

static void binEchoEnd() {
  // Handle zero-length echo (binEchoPayloadByte was never called)
  if (!echoBuffer && binPayloadLen == 0) {
    echoBufferIndex = 0;
  } else if (!echoBuffer) {
    return;  // allocation failed earlier
  }

  // Build response frame: \0 + header + payload + crc32
  uint8_t header[9];
  header[0] = BIN_PROTOCOL_VERSION;
  header[1] = BIN_CMD_ECHO & 0xFF;
  header[2] = (BIN_CMD_ECHO >> 8) & 0xFF;
  header[3] = 0;  // flags low
  header[4] = 0;  // flags high
  header[5] = echoBufferIndex & 0xFF;
  header[6] = (echoBufferIndex >> 8) & 0xFF;
  header[7] = (echoBufferIndex >> 16) & 0xFF;
  header[8] = (echoBufferIndex >> 24) & 0xFF;

  uint32_t crc = esp_crc32_le(0, header, 9);
  if (echoBufferIndex > 0 && echoBuffer) {
    crc = esp_crc32_le(crc, echoBuffer, echoBufferIndex);
  }

  uint8_t escape = 0x00;
  usbSerial.write(&escape, 1);
  usbSerial.write(header, 9);
  if (echoBufferIndex > 0 && echoBuffer) {
    usbSerial.write(echoBuffer, echoBufferIndex);
  }
  uint8_t crcBytes[4] = {
    (uint8_t)(crc & 0xFF),
    (uint8_t)((crc >> 8) & 0xFF),
    (uint8_t)((crc >> 16) & 0xFF),
    (uint8_t)((crc >> 24) & 0xFF)
  };
  usbSerial.write(crcBytes, 4);
  usbSerial.flush();

  free(echoBuffer);
  echoBuffer = nullptr;
  echoBufferSize = 0;
  echoBufferIndex = 0;
}

// Route payload bytes to the appropriate command handler
static void binProcessPayloadByte(uint8_t byte) {
  switch (binCmd) {
    case BIN_CMD_IMAGE:
      binImagePayloadByte(byte);
      break;
    case BIN_CMD_ECHO:
      binEchoPayloadByte(byte);
      break;
    default:
      // Unknown command — just consume bytes, error sent at dispatch
      break;
  }
}

//void onSerialDataReceived(const uint8_t* data, size_t length){
void onSerialDataReceived(){
  if (millis() - lastSerialTime > SERIAL_BUFFER_TIMEOUT_MS) {
    binReset();
  }
  lastSerialTime = millis();

  // Process each byte
  for (size_t i = 0; i < rawBufIndex; i++) {
    uint8_t c = (uint8_t)rawBuffer[i];

    switch (parseState) {
    case PARSE_TEXT:
      if (c == 0x00) {
        // Binary escape — switch to binary header mode
        parseState = PARSE_BIN_HEADER;
        binHeaderIndex = 0;
        binRunningCRC = 0;
        break;
      }

      // Normal text/JSON path
      if (bufferIndex >= MAX_BUFFER_SIZE - 1) {
          bufferIndex = 0;
          Serial.println("{\"jsonrpc\": \"2.0\", \"error\": {\"code\": -32700, \"message\": \"Buffer overflow\"}}");
          return;
      }
      inputBuffer[bufferIndex++] = (char)c;
      if (c == '\n') {
          inputBuffer[bufferIndex] = '\0';
          dataReceived = true;
          bufferIndex = 0;
      }
      break;

    case PARSE_BIN_HEADER:
      binHeader[binHeaderIndex++] = c;
      binRunningCRC = esp_crc32_le(binRunningCRC, &c, 1);

      if (binHeaderIndex == 9) {
        // Parse header fields (all little-endian)
        uint8_t version = binHeader[0];
        binCmd   = binHeader[1] | (binHeader[2] << 8);
        binFlags = binHeader[3] | (binHeader[4] << 8);
        binPayloadLen = binHeader[5] | (binHeader[6] << 8) |
                       (binHeader[7] << 16) | (binHeader[8] << 24);

        if (version != BIN_PROTOCOL_VERSION) {
          ESP_LOGW(TAG, "Binary: unsupported version %u", version);
          binSendResponse(binCmd, "unsupported version", false);
          binReset();
          break;
        }
        if (binPayloadLen > BIN_MAX_PAYLOAD) {
          ESP_LOGW(TAG, "Binary: payload too large %u", binPayloadLen);
          binSendResponse(binCmd, "payload too large", false);
          binReset();
          break;
        }

        binPayloadReceived = 0;
        imgSubHeaderIndex = 0;

        if (binPayloadLen == 0) {
          // No payload — go straight to checksum
          parseState = PARSE_BIN_CHECKSUM;
          binChecksumIndex = 0;
        } else {
          parseState = PARSE_BIN_PAYLOAD;
        }
      }
      break;

    case PARSE_BIN_PAYLOAD:
      binRunningCRC = esp_crc32_le(binRunningCRC, &c, 1);
      binProcessPayloadByte(c);
      binPayloadReceived++;

      if (binPayloadReceived >= binPayloadLen) {
        parseState = PARSE_BIN_CHECKSUM;
        binChecksumIndex = 0;
      }
      break;

    case PARSE_BIN_CHECKSUM:
      binChecksumBuf[binChecksumIndex++] = c;
      if (binChecksumIndex == BIN_CHECKSUM_SIZE) {
        uint32_t expected = binChecksumBuf[0] | (binChecksumBuf[1] << 8) |
                           (binChecksumBuf[2] << 16) | (binChecksumBuf[3] << 24);
        if (expected != binRunningCRC) {
          ESP_LOGW(TAG, "Binary: checksum mismatch (expected 0x%08X, got 0x%08X)",
                   expected, binRunningCRC);
          binImageEnd(false);
          binSendResponse(binCmd, "checksum mismatch", false);
        } else {
          binDispatch();
        }
        binReset();
      }
      break;
    }
  }
}

// Function to process JSON-RPC message
void processJsonRpcMessage(const char* jsonString) {
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, jsonString);
  //ESP_LOGI(TAG,"%s",jsonString);
  if (error) {
    String err = "{\"status\": \"error\", \"data\": {\"code\": -32700, \"message\": \"Parse error "+String(error.c_str())+"\"}}";
    printErr(err);    
    return;
  }
  
  if (!doc["action"] || !doc["params"]) {  
    String err = "{\"status\": \"error\", \"data\": {\"code\": -32600, \"message\": \"Invalid request\"}}";
    printErr(err);
    return;
  }

  String action = doc["action"].as<String>();
  JsonDocument responseDoc;
  JsonObject result = responseDoc.to<JsonObject>();  
  
  if(action == "set"){

    JsonObject params = doc["params"].as<JsonObject>();
    bool pFail = false;
    
  
    if(params["startUpmode"]){
      int inx = getEnumIndex(params["startUpmode"].as<const char*>(),t_startupMode,ARR_SIZE(t_startupMode));
      inx != -1 ? gloConfig->features.startUpmode = inx : result["startUpmode"] = "fail";
    }
    if(params["wifi_enabled"]) {
      int inx = getEnumIndex(params["wifi_enabled"].as<const char*>(),t_bool,ARR_SIZE(t_bool));
      inx != -1 ? gloConfig->features.wifi_enabled = inx : result["wifi_enabled"] = "fail";
    }
    if(params["hubMode"]){
      int inx = getEnumIndex(params["hubMode"].as<const char*>(),t_hubMode,ARR_SIZE(t_hubMode));
      inx != -1 ? gloConfig->features.hubMode = inx : result["hubMode"] = "fail";
      //delay to allow other json elements downstream not to be overwritten
      vTaskDelay(pdMS_TO_TICKS(150)); 
    }        
    if(params["filterType"]){
      int inx = getEnumIndex(params["filterType"].as<const char*>(),t_filterType,ARR_SIZE(t_filterType));
      inx != -1 ? gloConfig->features.filterType = inx : result["filterType"] = "fail";
    }
    if(params["refreshRate"]){
      int inx = getEnumIndex(params["refreshRate"].as<const char*>(),t_refreshRate,ARR_SIZE(t_refreshRate));
      inx != -1 ? gloConfig->features.refreshRate = inx : result["refreshRate"] = "fail";
    }        
    if(params["rotation"]){
      int inx = getEnumIndex(params["rotation"].as<const char*>(),t_rotation,ARR_SIZE(t_rotation));
      if(inx != -1) {
        gloConfig->screen[0].rotation = inx;
        gloConfig->screen[1].rotation = inx;
        gloConfig->screen[2].rotation = inx;
      }
      else
        result["rotation"] = "fail";
    }   
    if(params.containsKey("brightness")){
      uint16_t inx = params["brightness"].as<unsigned int>();
      //5% is minimum brighness. A complete dark display(0%) may lead to confusion and make
      //difficult to restore using the UIH menu 
      if(inx >= 5 && inx <= 100) {
        uint16_t pwm = brightnessPctToPwm(inx);
        gloConfig->screen[0].brightness = pwm;
        gloConfig->screen[1].brightness = pwm;
        gloConfig->screen[2].brightness = pwm;
      }
      else
        result["brightness"] = "out of range (5-100% expected)";
    }


    if(params.containsKey("reboot_enabled")){
      uint8_t val = params["reboot_enabled"].as<unsigned int>();
      if(val <= 1) {
        gloConfig->features.reboot_enabled = val;
        usbSerial.enableReboot(val == ENABLE);
      } else {
        result["reboot_enabled"] = "fail";
      }
    }


    if(params["ledState"]){
      int inx = getEnumIndex(params["ledState"].as<const char*>(),t_bool,ARR_SIZE(t_bool));
      inx != -1 ? gloState->system.ledState = inx : result["ledState"] = "fail";        
    }

    if(params["autoTxtClear"]){
      int inx = getEnumIndex(params["autoTxtClear"].as<const char*>(),t_bool,ARR_SIZE(t_bool));
      inx != -1 ? gloState->features.enableClearScreenText = inx : result["autoTxtClear"] = "fail";        
    }

    for(int i = 0; i<3; i++){

      if(params["CH"+String(i+1)]["powerEn"]){
        int inx = getEnumIndex(params["CH"+String(i+1)]["powerEn"].as<const char*>(),t_bool,ARR_SIZE(t_bool));
        inx != -1 ? gloState->baseMCUOut[i].pwr_en = inx : result["CH"+String(i+1)]["powerEn"] = "fail";        
      }
      if(params["CH"+String(i+1)]["dataEn"]){
        int inx = getEnumIndex(params["CH"+String(i+1)]["dataEn"].as<const char*>(),t_bool,ARR_SIZE(t_bool));
        inx != -1 ? gloState->baseMCUOut[i].data_en = inx :  result["CH"+String(i+1)]["dataEn"] = "fail";        
      }      
      if(params["CH"+String(i+1)]["startup_tmr"]){
        uint8_t inx = params["CH"+String(i+1)]["startup_tmr"].as<unsigned int>();
        (inx >= 1 && inx <= 100) ? gloConfig->startup[i].startup_timer = inx :  result["CH"+String(i+1)]["startup_tmr"] = "out of range";
      }
      if(params["CH"+String(i+1)]["fwdLimit"]){
        uint16_t inx = params["CH"+String(i+1)]["fwdLimit"].as<unsigned int>();
        (inx >= 100 && inx <= 2000) ? gloConfig->meter[i].fwdCLim = inx : result["CH"+String(i+1)]["fwdLimit"] = "out of range";
      }
      if(params["CH"+String(i+1)]["backLimit"]){
        uint16_t inx = params["CH"+String(i+1)]["backLimit"].as<unsigned int>();
        (inx >= 1 && inx <= 200) ? gloConfig->meter[i].backCLim = inx : result["CH"+String(i+1)]["backLimit"] = "out of range";
      }

      if(params["CH"+String(i+1)]["fwdAlert"]){
        int inx = getEnumIndex(params["CH"+String(i+1)]["fwdAlert"].as<const char*>(),t_bool,ARR_SIZE(t_bool));
        inx != -1 ? gloState->meter[i].fwdAlertSet = inx : result["CH"+String(i+1)]["fwdAlert"] = "fail";        
      }
      if(params["CH"+String(i+1)]["backAlert"]){
        int inx = getEnumIndex(params["CH"+String(i+1)]["backAlert"].as<const char*>(),t_bool,ARR_SIZE(t_bool));
        inx != -1 ? gloState->meter[i].backAlertSet = inx : result["CH"+String(i+1)]["backAlert"] = "fail";        
      }
      if(params["CH"+String(i+1)]["shortAlert"]){
        int inx = getEnumIndex(params["CH"+String(i+1)]["shortAlert"].as<const char*>(),t_bool,ARR_SIZE(t_bool));
        inx != -1 ? gloState->baseMCUIn[i].fault = inx : result["CH"+String(i+1)]["shortAlert"] = "fail";        
      }          
      
      if(params["CH"+String(i+1)]["numDev"]){
        uint8_t inx = params["CH"+String(i+1)]["numDev"].as<unsigned int>();
        (inx >= 0 && inx <= 11) ? gloState->usbInfo[i].numDev = inx : result["CH"+String(i+1)]["numDev"] = "out of range";
      }
      if(params["CH"+String(i+1)]["Dev1_name"]){
        gloState->usbInfo[i].Dev1_Name = params["CH"+String(i+1)]["Dev1_name"].as<String>();        
      }
      if(params["CH"+String(i+1)]["Dev2_name"]){
        gloState->usbInfo[i].Dev2_Name = params["CH"+String(i+1)]["Dev2_name"].as<String>();        
      }
      if(params["CH"+String(i+1)]["usbType"]){
        uint8_t inx = params["CH"+String(i+1)]["usbType"].as<unsigned int>();
        (inx >= 0 && inx <= 3) ? gloState->usbInfo[i].usbType = inx : result["CH"+String(i+1)]["usbType"] = "out of range";
      }

    }
    
    result["valid"] = String(params.size()-result.size()) + " of " + String(params.size());
    
    sendJsonResponse(0, result);
   
  }  

  if(action == "get") {  
    JsonArray params = doc["params"].as<JsonArray>();
    JsonDocument responseDoc;
    JsonObject result = responseDoc.to<JsonObject>();
    

    for (JsonVariant v : params) {
      String pName = v.as<String>();
      
      bool all, conf, state;
      
      pName == "all" ? all = true : all = false;
      pName == "config" ? conf = true : conf = false;
      pName == "state" ? state = true: state = false;
      
      if(pName == "startUpmode"   || all || conf)  
        result["startUpmode"]   = t_startupMode[gloConfig->features.startUpmode];
      if(pName == "wifi_enabled"  || all || conf) 
        result["wifi_enabled"]  = gloConfig->features.wifi_enabled;
      if(pName == "hubMode"       || all || conf)      
        result["hubMode"]       = t_hubMode[gloConfig->features.hubMode];
      if(pName == "filterType"    || all || conf)   
        result["filterType"]    = t_filterType[gloConfig->features.filterType];
      if(pName == "refreshRate"   || all || conf)  
        result["refreshRate"]   = t_refreshRate[gloConfig->features.refreshRate];
      if(pName == "rotation"      || all || conf)     
        result["rotation"]      = t_rotation[gloConfig->screen[0].rotation];
      if(pName == "brightness"    || all || conf)
        result["brightness"]    = brightnessPwmToPct(gloConfig->screen[0].brightness);
      if(pName == "reboot_enabled" || all || conf)
        result["reboot_enabled"] = gloConfig->features.reboot_enabled;


      if(pName == "startUpActive" || all || state)
        result["startUpActive"] = gloState->features.startUpActive;
      if(pName == "pcConnected"   || all || state)  
        result["pcConnected"]   = gloState->features.pcConnected;
      if(pName == "vbus"   || all || state)  
        result["vbus"]   = String(gloState->features.vbus,0);
      if(pName == "vext_cc"       || all || state)      
        result["vext_cc"]       = t_vx_cc[gloState->baseMCUExtra.vext_cc];
      if(pName == "vhost_cc"      || all || state)     
        result["vhost_cc"]      = t_vx_cc[gloState->baseMCUExtra.vhost_cc];
      if(pName == "vext_stat"     || all || state)    
        result["vext_stat"]     = t_vx_stat[gloState->baseMCUExtra.vext_stat];
      if(pName == "vhost_stat"    || all || state)   
        result["vhost_stat"]    = t_vx_stat[gloState->baseMCUExtra.vhost_stat];
      if(pName == "pwr_source"    || all || state)   
        gloState->baseMCUExtra.pwr_source ? result["pwr_source"] = "vext": result["pwr_source"] = "vhost";
      if(pName == "usb3_mux_out_en" || all || state) 
        result["usb3_mux_out_en"] = gloState->baseMCUExtra.usb3_mux_out_en;
      if(pName == "usb3_mux_sel_pos" || all || state) 
        gloState->baseMCUExtra.usb3_mux_sel_pos ? result["usb3_mux_sel_pos"] = "1" : result["usb3_mux_sel_pos"] = "0";
      if(pName == "base_ver"      || all || state)     
        result["base_ver"]      = gloState->baseMCUExtra.base_ver;
      if(pName == "esp32_ver"     || all || state)
        result["cpu_ver"]       = APP_VERSION;
      if(pName == "mac"           || all || state)
        result["mac"]           = gloState->system.wifiMAC; 
      if(pName == "autoTxtClear"    || all || state)
        result["autoTxtClear"]      = gloState->features.enableClearScreenText;        
      
      //production specific getters
    
      if(pName == "cpu_freq"    || all || state)
        result["cpu_freq"]      = String(ESP.getCpuFreqMHz());
      if(pName == "ledState"    || all || state)
        result["ledState"]      = gloState->system.ledState;     
      if(pName == "firstStart"    || all || state){
        result["firstStart"]    = gloState->system.firstStart;
        gloState->system.firstStart = false;
      }
      if(pName == "menuIsActive"    || all || state)
        result["menuIsActive"]      = gloState->system.menuIsActive;
      if(pName == "meterInit"    || all || state){
        result["meterInit"]      =  t_meter_init[gloState->system.meterInit];
      }  
      if(pName == "pacRev"    || all || state)
        result["pacRev"]      = String(gloState->system.pacRevisionID);
      if(pName == "uptime"    || all || state)
        result["uptime"]      = millis();

      for(int i = 0; i<3; i++){
        if (pName == "CH"+String(i+1) || pName == "CH"+String(i+1)+"_all"){
          result["CH"+String(i+1)]["voltage"]     = String(gloState->meter[i].AvgVoltage,1);
          result["CH"+String(i+1)]["current"]     = String(gloState->meter[i].AvgCurrent,1);
          result["CH"+String(i+1)]["fwdAlert"]    = gloState->meter[i].fwdAlertSet;
          result["CH"+String(i+1)]["backAlert"]   = gloState->meter[i].backAlertSet;
          result["CH"+String(i+1)]["shortAlert"]  = gloState->baseMCUIn[i].fault;          
          result["CH"+String(i+1)]["dataEn"]      = gloState->baseMCUOut[i].data_en;
          result["CH"+String(i+1)]["powerEn"]     = gloState->baseMCUOut[i].pwr_en;
        }
        if(pName == "CH"+String(i+1)+"_all"){
          result["CH"+String(i+1)]["ilim"]        = gloState->baseMCUOut[i].ilim;
          result["CH"+String(i+1)]["startup_cnt"] = gloState->startup[i].startup_cnt;
          result["CH"+String(i+1)]["startup_tmr"] = gloConfig->startup[i].startup_timer;
          result["CH"+String(i+1)]["fwdLimit"]    = gloConfig->meter[i].fwdCLim;
          result["CH"+String(i+1)]["backLimit"]   = gloConfig->meter[i].backCLim;
          result["CH"+String(i+1)]["numDev"]      = gloState->usbInfo[i].numDev;
          result["CH"+String(i+1)]["Dev1_name"]   = gloState->usbInfo[i].Dev1_Name;
          result["CH"+String(i+1)]["Dev2_name"]   = gloState->usbInfo[i].Dev2_Name;
          result["CH"+String(i+1)]["usbType"]     = gloState->usbInfo[i].usbType;
        } 

      }

    }

    sendJsonResponse(0, result);
  }
  else if(action == "restart"){
    JsonObject params = doc["params"].as<JsonObject>();
    bool immediate = params["immediate"] | false;
    sendJsonResponse(0, result);
    usbSerial.flush();
    if(immediate){
      delay(100);
      ESP.restart();
    } else {
      RestartService::restartNow();
    }
  }

}

// Send JSON-RPC response
void sendJsonResponse(int id, JsonVariant result) {
  JsonDocument doc;
  doc["status"] = "ok";
  doc["data"] = result;

  String response;
  serializeJson(doc, response);
  usbSerial.println(response);
  usbSerial.flush();
}

void printErr(String err){
  
  usbSerial.println(err);
  usbSerial.flush();
  ESP_LOGI(TAG," %s", err.c_str());
}

int getEnumIndex(const char* name, const char* const* array, int size) {
  for (int i = 0; i < size; ++i) {
      //ESP_LOGI(TAG,"size: %u, name: %s, array: %s, i: %u", size,name,array[i],i);
      if (strcmp(array[i], name) == 0) {
          return i;
      }
  }
  return -1; // Return -1 if not found
}

//Handlers for TinyUSB events when working in CDC mode

static void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
  if(event_base == ARDUINO_USB_EVENTS)
  {
    arduino_usb_event_data_t * data = (arduino_usb_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_STARTED_EVENT:
        //HWSerial.println("USB PLUGGED");
        ESP_LOGI(TAG,"USB PLUGGED");
        gloState->features.usbHostState = USB_PLUGGED;
        break;
      case ARDUINO_USB_STOPPED_EVENT:
        //HWSerial.println("USB UNPLUGGED");
        ESP_LOGI(TAG,"USB UNPLUGGED");
        gloState->features.usbHostState = USB_UNPLUGGED;
        break;
      case ARDUINO_USB_SUSPEND_EVENT:
        //HWSerial.printf("USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en);
        ESP_LOGI(TAG,"USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en);
        gloState->features.usbHostState = USB_SUSPENDED;
        break;
      case ARDUINO_USB_RESUME_EVENT:
        //HWSerial.println("USB RESUMED");
        ESP_LOGI(TAG,"USB RESUMED");
        gloState->features.usbHostState = USB_RESUME;
        break;
      
      default:
        break;
    }
  } else if(event_base == ARDUINO_USB_CDC_EVENTS)
  {
    arduino_usb_cdc_event_data_t * data = (arduino_usb_cdc_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_CDC_CONNECTED_EVENT:        
        ESP_LOGI(TAG,"CDC CONNECTED");
        break;
      case ARDUINO_USB_CDC_DISCONNECTED_EVENT: 
        ESP_LOGI(TAG,"CDC DISCONNECTED");
        break;
      case ARDUINO_USB_CDC_LINE_STATE_EVENT:
        ESP_LOGI(TAG,"CDC LINE STATE: dtr: %u, rts: %u\n", data->line_state.dtr, data->line_state.rts);
        break;
      case ARDUINO_USB_CDC_LINE_CODING_EVENT:
        ESP_LOGI(TAG,"CDC LINE CODING: bit_rate: %u, data_bits: %u, stop_bits: %u, parity: %u\n", data->line_coding.bit_rate, data->line_coding.data_bits, data->line_coding.stop_bits, data->line_coding.parity);
        break;
      case ARDUINO_USB_CDC_RX_EVENT:
        //ESP_LOGV(TAG,"CDC RX [%u]:", data->rx.len);
        {
            //uint8_t buf[data->rx.len]; //redundant- left for reference

            rawBufIndex = usbSerial.read(rawBuffer,data->rx.len);
            onSerialDataReceived();

            USBSerialActivity=true;
            gloState->features.pcConnected = true;
            
        }
        break;
      case ARDUINO_USB_CDC_RX_OVERFLOW_EVENT:
        ESP_LOGW(TAG,"CDC RX Overflow of %d bytes", data->rx_overflow.dropped_bytes);
        break;
     
      default:
        break;
    }
  }
}
