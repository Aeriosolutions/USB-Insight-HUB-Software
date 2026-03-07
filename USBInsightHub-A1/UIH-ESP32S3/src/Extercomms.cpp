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
#include <esp_heap_caps.h>
#include "freertos/stream_buffer.h"

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
volatile unsigned long screenLockTime[3] = {0, 0, 0};
volatile uint8_t screenReadyPending = 0;
SemaphoreHandle_t screenReadySemaphore = NULL;

char rawBuffer[2048];
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
static unsigned long binLastByteTime = 0;  // millis() of last byte in binary state
#define BIN_TIMEOUT_MS 2000               // reset binary parser after 2s of silence
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
static uint8_t imgPort = 0;
static uint8_t imgBpp = 0;
static uint16_t imgWidth = 0;
static uint16_t imgHeight = 0;
static uint32_t imgPixelsExpected = 0;
// imgFailed removed — error handling is now centralized in binFrameFailed.
// Handlers call binSendResponse() which sets binFrameFailed; the common parser
// then stops forwarding payload bytes and skips dispatch.
static uint16_t imgFlags = 0;  // raw flags from frame header
static uint16_t imgMode = 0;   // low 2 bits: buffer/sprite/direct
static bool imgRLE = false;     // bit 2: RLE-compressed payload

// RLE decoder state — carries across payload blocks
// Phase: 0=expecting count byte, 1=expecting value byte, 2=expanding run
static uint8_t imgRLEPhase = 0;
static uint8_t imgRLECount = 0;  // remaining run length
static uint8_t imgRLEValue = 0;  // current run value

// Direct mode state — SPI streaming during receive
static int imgDirectCSPin = -1;
static bool imgDirectActive = false;
static bool imgDirectSemHeld = false;  // track if we're holding screen_Semaphore
static uint8_t imgDirectCarry = 0;     // leftover byte for 16bpp cross-block alignment
static bool imgDirectHasCarry = false;

// Sprite mode — viewport coordinates for writing into sprite buffer
#define VIEWPORT_X 7
#define VIEWPORT_Y 40
#define VIEWPORT_W 226
#define VIEWPORT_H 90

// Pixel data buffer — accumulated during receive, flushed to SPI after CRC verified
static uint8_t* imgPixelBuf = nullptr;
static size_t imgPixelBufSize = 0;
static size_t imgPixelBufIndex = 0;

// Image transfer profiling
static struct {
  unsigned long t0;           // frame begin (header parsed) — micros()
  unsigned long tReceived;    // all payload bytes buffered
  unsigned long tCrcDone;     // CRC verified, dispatch called
  unsigned long tSemAcq;      // screen semaphore acquired
  unsigned long tSpiDone;     // SPI render complete
  uint32_t drainReads;        // usbSerial.read() calls during this image
  uint32_t drainBytes;        // total bytes read from USB
  uint32_t drainWakes;        // task wakeups during image receive
  bool active;                // profiling in progress
  void reset() { memset(this, 0, sizeof(*this)); }
} imgProf;

// CRC-32 using ESP32 ROM hardware acceleration (esp_crc32_le)
// No lookup table needed — uses the chip's built-in CRC unit.

// Echo command state
static uint8_t* echoBuffer = nullptr;
static size_t echoBufferSize = 0;
static size_t echoBufferIndex = 0;

// Meter stream state
static volatile bool meterStreamActive = false;
static volatile uint8_t meterStreamChannelMask = 0;  // bits 0-2 = CH1-3
static volatile uint16_t meterStreamIntervalMs = 100;
static unsigned long meterStreamLastSend = 0;

// Meter subscribe payload: channel_mask(1) + interval_ms(2,LE)
static uint8_t meterSubPayload[3];
static size_t meterSubPayloadIndex = 0;

// Pluggable binary command handler registry
static const BinCommandHandler* binHandlers[BIN_MAX_HANDLERS] = {};
static size_t binHandlerCount = 0;
static const BinCommandHandler* binActiveHandler = nullptr;

TaskHandle_t exterTaskHandle = nullptr;

// --- Fast CDC RX path: bypass Arduino per-byte queue via --wrap linker intercept ---
static StreamBufferHandle_t cdcRxStream = NULL;
#define CDC_STREAM_SIZE 4096

extern "C" void __real_tud_cdc_rx_cb(uint8_t itf);

extern "C" void __wrap_tud_cdc_rx_cb(uint8_t itf) {
    if (itf != 0 || !cdcRxStream) {
        __real_tud_cdc_rx_cb(itf);  // fallback to Arduino during early boot
        return;
    }
    // Drain TinyUSB FIFO into StreamBuffer in one bulk operation
    uint8_t buf[CONFIG_TINYUSB_CDC_RX_BUFSIZE];
    uint32_t count = tud_cdc_n_read(itf, buf, sizeof(buf));
    if (count > 0) {
        xStreamBufferSend(cdcRxStream, buf, count, 0);
        USBSerialActivity = true;
        if (exterTaskHandle) xTaskNotifyGive(exterTaskHandle);
    }
}

// --- Fix ESP32-S3 USB bootloader entry ---
// Arduino's usb_persist_enabled is commented out (esp32-hal-tinyusb.c:566) and
// Espressif confirmed USB persist "does not work on S3 at all." The problem is
// that TinyUSB holds the USB PHY pins, preventing the host from detecting the
// disconnect/reconnect during usb_switch_to_cdc_jtag(). Community workaround:
// call tud_disconnect() and release GPIO 19/20 before usb_persist_restart().
//
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

static void enterBootloader() {
    usb_persist_restart(2);  // RESTART_BOOTLOADER — goes through our wrapper
}

void binRegisterCommand(const BinCommandHandler* handler) {
  if (binHandlerCount < BIN_MAX_HANDLERS) {
    binHandlers[binHandlerCount++] = handler;
  }
}

static const BinCommandHandler* binFindHandler(uint16_t cmd) {
  for (size_t i = 0; i < binHandlerCount; i++) {
    if (binHandlers[i]->cmd == cmd) return binHandlers[i];
  }
  return nullptr;
}

// Forward declarations
static void binReset();
static void binImageBegin();
static void binImagePayloadByte(uint8_t byte);
static void binImagePayloadBlock(const uint8_t* data, size_t len);
static void binImageRender();
static void binImageCleanup();
static void binImageFreeBuffer();
static void binEchoBegin(uint32_t payloadLen);
static void binEchoPayloadByte(uint8_t byte);
static void binEchoPayloadBlock(const uint8_t* data, size_t len);
static void binEchoEnd();
static void binMeterStreamPayloadByte(uint8_t byte);
static void binMeterStreamDispatch();
static void binMeterStreamSendSample();
static void binSendBinaryFrame(uint16_t cmd, const uint8_t* payload, size_t len);
static void binSendResponse(uint16_t cmd, const char* message, bool ok = true);
static void binFlushPendingResponse();

// --- Pluggable command handler structs ---

static const BinCommandHandler imageHandler = {
  .cmd = BIN_CMD_IMAGE,
  .begin = [](uint16_t, uint16_t flags, uint32_t) -> bool {
    imgSubHeaderIndex = 0;
    imgPixelBufIndex = 0;
    imgFlags = flags;
    imgMode = flags & 0x03;
    imgRLE = (flags & IMG_FLAG_RLE) != 0;
    imgRLEPhase = 0;
    imgRLECount = 0;
    imgDirectActive = false;
    imgDirectSemHeld = false;
    imgDirectCSPin = -1;
    imgDirectHasCarry = false;
    imgProf.reset();
    imgProf.t0 = micros();
    imgProf.active = true;
    // Validate flags — only mode bits [1:0] and RLE bit [2] are defined
    if (imgMode > IMG_FLAG_DIRECT || (flags & ~0x07) != 0) {
      binSendResponse(BIN_CMD_IMAGE, "invalid flags", false);
      return false;
    }
    return true;
  },
  .payloadByte = binImagePayloadByte,
  .payloadBlock = binImagePayloadBlock,
  .dispatch = []() {
    binImageRender();
  },
  .reset = []() {
    // Clean up direct mode if in progress (timeout/error recovery)
    if (imgDirectActive && gloScreen) {
      gloScreen->streamEnd();
      imgDirectActive = false;
    }
    if (imgDirectSemHeld) {
      xSemaphoreGive(screen_Semaphore);
      imgDirectSemHeld = false;
    }
    // Don't clear imageMode here — it's cleared by the 10s screen lock
    // timeout or PC disconnect. Clearing it between frames causes DefaultView
    // to overwrite the viewport with the default chrome.
    imgPixelBufIndex = 0;
    imgSubHeaderIndex = 0;
    imgFlags = 0;
    imgMode = 0;
    imgRLE = false;
    imgRLEPhase = 0;
    imgRLECount = 0;
    imgDirectHasCarry = false;
  },
};

static void binEchoPayloadBlock(const uint8_t* data, size_t len) {
  if (echoBuffer && echoBufferIndex + len <= echoBufferSize) {
    memcpy(echoBuffer + echoBufferIndex, data, len);
    echoBufferIndex += len;
  }
}

static const BinCommandHandler echoHandler = {
  .cmd = BIN_CMD_ECHO,
  .begin = [](uint16_t, uint16_t, uint32_t payloadLen) -> bool {
    binEchoBegin(payloadLen);
    return echoBuffer != nullptr || payloadLen == 0;
  },
  .payloadByte = binEchoPayloadByte,
  .payloadBlock = binEchoPayloadBlock,
  .dispatch = binEchoEnd,
  .reset = []() {
    free(echoBuffer);
    echoBuffer = nullptr;
    echoBufferSize = 0;
    echoBufferIndex = 0;
  },
};

static const BinCommandHandler meterHandler = {
  .cmd = BIN_CMD_METER_STREAM,
  .begin = [](uint16_t, uint16_t, uint32_t) -> bool {
    meterSubPayloadIndex = 0;
    return true;
  },
  .payloadByte = binMeterStreamPayloadByte,
  .payloadBlock = nullptr,
  .dispatch = binMeterStreamDispatch,
  .reset = nullptr,
};

// --- Screen lock command (cmd=0x0004) ---
static uint8_t screenLockPayload[2];
static size_t screenLockPayloadIndex = 0;

static const BinCommandHandler screenLockHandler = {
  .cmd = BIN_CMD_SCREEN_LOCK,
  .begin = [](uint16_t, uint16_t, uint32_t payloadLen) -> bool {
    screenLockPayloadIndex = 0;
    if (payloadLen != 2) {
      binSendResponse(BIN_CMD_SCREEN_LOCK, "payload must be 2 bytes", false);
      return false;
    }
    return true;
  },
  .payloadByte = [](uint8_t byte) {
    if (screenLockPayloadIndex < sizeof(screenLockPayload))
      screenLockPayload[screenLockPayloadIndex++] = byte;
  },
  .payloadBlock = nullptr,
  .dispatch = []() {
    uint8_t mask = screenLockPayload[0];
    uint8_t action = screenLockPayload[1];
    if (mask == 0 || mask > 0x07) {
      binSendResponse(BIN_CMD_SCREEN_LOCK, "invalid channel mask", false);
      return;
    }
    if (action > 1) {
      binSendResponse(BIN_CMD_SCREEN_LOCK, "invalid action (0 or 1)", false);
      return;
    }
    unsigned long now = millis();
    for (int i = 0; i < 3; i++) {
      if (mask & (1 << i)) {
        if (action == 1) {
          imageMode[i] = true;
          screenLockTime[i] = now;
        } else {
          imageMode[i] = false;
          screenLockTime[i] = 0;
        }
      }
    }
    char msg[48];
    snprintf(msg, sizeof(msg), "%s mask=0x%02X", action ? "locked" : "unlocked", mask);
    binSendResponse(BIN_CMD_SCREEN_LOCK, msg);
  },
  .reset = nullptr,
};

// --- Screen ready command (cmd=0x0005) ---
static uint8_t screenReadyChannel = 0;

static const BinCommandHandler screenReadyHandler = {
  .cmd = BIN_CMD_SCREEN_READY,
  .begin = [](uint16_t, uint16_t, uint32_t payloadLen) -> bool {
    screenReadyChannel = 0;
    if (payloadLen != 1) {
      binSendResponse(BIN_CMD_SCREEN_READY, "payload must be 1 byte", false);
      return false;
    }
    return true;
  },
  .payloadByte = [](uint8_t byte) {
    screenReadyChannel = byte;
  },
  .payloadBlock = nullptr,
  .dispatch = []() {
    if (screenReadyChannel < 1 || screenReadyChannel > 3) {
      binSendResponse(BIN_CMD_SCREEN_READY, "invalid channel (1-3)", false);
      return;
    }
    uint8_t idx = screenReadyChannel - 1;
    if (!imageMode[idx]) {
      binSendResponse(BIN_CMD_SCREEN_READY, "channel not locked", false);
      return;
    }
    // Refresh lock timeout
    screenLockTime[idx] = millis();
    // Set pending bit and wait for render loop to signal
    screenReadyPending |= (1 << idx);
    if (xSemaphoreTake(screenReadySemaphore, pdMS_TO_TICKS(250)) == pdTRUE) {
      screenReadyPending &= ~(1 << idx);
      binSendResponse(BIN_CMD_SCREEN_READY, "ready");
    } else {
      screenReadyPending &= ~(1 << idx);
      binSendResponse(BIN_CMD_SCREEN_READY, "timeout", false);
    }
  },
  .reset = nullptr,
};

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

    cdcRxStream = xStreamBufferCreate(CDC_STREAM_SIZE, 1);
    usbSerial.setRxBufferSize(256);  // Arduino queue only used as fallback before StreamBuffer ready
    usbSerial.begin(115200);
    usbSerial.enableReboot(globalConfig->features.reboot_enabled == ENABLE);
    USB.manufacturerName("Aerio");
    USB.productName("InsightHUB Controller");
    USB.begin();

    // Register pluggable binary command handlers
    binRegisterCommand(&imageHandler);
    binRegisterCommand(&echoHandler);
    binRegisterCommand(&meterHandler);
    binRegisterCommand(&screenLockHandler);
    binRegisterCommand(&screenReadyHandler);

    screenReadySemaphore = xSemaphoreCreateBinary();

    xTaskCreatePinnedToCore(taskExterCheckActivity, "Extercom check", 6144, NULL, 5, &exterTaskHandle, APP_CORE);

}

void iniExtercommsBinaryTransport(Screen* screen){
  gloScreen = screen;
}

//serial loop - check if there has been serial activity to update the pc-connection status icon
void taskExterCheckActivity(void *pvParameters){
    unsigned long lastPCcom = 0;
    unsigned long now;
    for(;;){
        // Wait for CDC RX notification or timeout for periodic tasks
        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(SERIAL_CHECK_PERIOD));

        now = millis();

        if (imgProf.active) imgProf.drainWakes++;

        // Drain CDC StreamBuffer — tight loop until empty
        while ((rawBufIndex = xStreamBufferReceive(cdcRxStream, rawBuffer, sizeof(rawBuffer), 0)) > 0) {
          if (imgProf.active) {
            imgProf.drainReads++;
            imgProf.drainBytes += rawBufIndex;
          }
          onSerialDataReceived();
        }

        // Binary parse timeout — recover if sender drops mid-frame
        if (parseState != PARSE_TEXT && (millis() - binLastByteTime > BIN_TIMEOUT_MS)) {
          ESP_LOGW(TAG, "Binary: timeout in state %d cmd=%u payload=%u/%u",
                   parseState, binCmd, binPayloadReceived, binPayloadLen);
          binSendResponse(binCmd, "timeout", false);
          binReset();
          binFlushPendingResponse();
        }

        if(USBSerialActivity){
          lastPCcom = millis();
          gloState->features.pcConnected = true;
          gloState->features.clearScreenText = false;
          USBSerialActivity=false;
        }

        if(millis()-lastPCcom > PC_CONNECTION_TIMEOUT){
          if (gloState->features.pcConnected) {
            // Release all screen locks on PC disconnect
            for (int i = 0; i < 3; i++) {
              if (imageMode[i]) {
                imageMode[i] = false;
                screenLockTime[i] = 0;
                ESP_LOGI(TAG, "Screen lock: CH%d released (PC disconnected)", i + 1);
              }
            }
            binImageFreeBuffer();  // Release pixel buffer on disconnect
          }
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

        // Screen lock timeout — auto-release if host hasn't refreshed
        for (int i = 0; i < 3; i++) {
          if (imageMode[i] && screenLockTime[i] != 0 &&
              (millis() - screenLockTime[i] > SCREEN_LOCK_TIMEOUT_MS)) {
            imageMode[i] = false;
            screenLockTime[i] = 0;
            ESP_LOGI(TAG, "Screen lock: CH%d auto-released (timeout)", i + 1);
          }
        }

        if(dataReceived){
          processJsonRpcMessage(inputBuffer);  // Process JSON
          dataReceived = false;
        }

        // Meter streaming — send samples at the configured interval
        if(meterStreamActive && (now - meterStreamLastSend >= meterStreamIntervalMs)){
          meterStreamLastSend = now;
          binMeterStreamSendSample();
        }
    }

}

// Deferred response — buffers the response JSON so it can be sent AFTER the
// binary frame is fully consumed (payload + checksum).  Sending during RX-heavy
// payload draining can stall the USB CDC TX path and cause the hub to hang.
static char binPendingResponse[128];
static bool binHasPendingResponse = false;

// Frame-level failure flag.  When a handler (or the parser) calls
// binSendResponse() with ok=false during binary frame processing, this flag
// is set.  The common parser code then:
//  - Stops forwarding payload bytes to the handler
//  - Skips dispatch() after checksum
//  - Sends the deferred error response after the frame is fully consumed
// Handlers that detect errors just call binSendResponse() and return — they
// don't need their own "failed" tracking or drain logic.
static bool binFrameFailed = false;

static void binReset() {
  if (binActiveHandler && binActiveHandler->reset) {
    binActiveHandler->reset();
  }
  binActiveHandler = nullptr;
  parseState = PARSE_TEXT;
  binHeaderIndex = 0;
  binPayloadLen = 0;
  binPayloadReceived = 0;
  binCmd = 0;
  binFlags = 0;
  binRunningCRC = 0;
  binChecksumIndex = 0;
  binFrameFailed = false;
}

static void binSendResponse(uint16_t cmd, const char* message, bool ok) {
  snprintf(binPendingResponse, sizeof(binPendingResponse),
    "{\"status\":\"%s\",\"data\":{\"cmd\":%u,\"message\":\"%s\"}}",
    ok ? "ok" : "error", cmd, message);

  // If we're in the middle of processing a binary frame, defer the send.
  // The response will be flushed after checksum verification in PARSE_BIN_CHECKSUM.
  if (parseState != PARSE_TEXT) {
    binHasPendingResponse = true;
    if (!ok) binFrameFailed = true;
    return;
  }

  // Not in binary frame — send immediately (e.g. from dispatch or timeout handler)
  usbSerial.println(binPendingResponse);
  usbSerial.flush();
  binHasPendingResponse = false;
}

// Flush any deferred response — called after binary frame is fully consumed.
static void binFlushPendingResponse() {
  if (binHasPendingResponse) {
    usbSerial.println(binPendingResponse);
    usbSerial.flush();
    binHasPendingResponse = false;
  }
}


// RLE decoder: expand RLE-encoded data into imgPixelBuf.
// RLE format: [count][value][count][value]...
// Phase state machine carries across calls:
//   phase 0: expecting count byte
//   phase 1: expecting value byte (count already stored)
//   phase 2: expanding run (count/value stored)
static void binImageRLEDecode(const uint8_t* data, size_t len) {
  if (!imgPixelBuf) return;
  size_t i = 0;
  while (i < len && imgPixelBufIndex < imgPixelBufSize) {
    switch (imgRLEPhase) {
      case 0:  // expecting count
        imgRLECount = data[i++];
        imgRLEPhase = 1;
        break;
      case 1:  // expecting value
        imgRLEValue = data[i++];
        imgRLEPhase = 2;
        break;
      case 2: {  // expanding run
        size_t space = imgPixelBufSize - imgPixelBufIndex;
        size_t n = imgRLECount;
        if (n > space) n = space;
        memset(imgPixelBuf + imgPixelBufIndex, imgRLEValue, n);
        imgPixelBufIndex += n;
        imgRLECount -= n;
        if (imgRLECount == 0) imgRLEPhase = 0;
        break;
      }
    }
  }
}

// RLE decoder for active direct mode: decode into stack buffer, stream to SPI.
static void binImageRLEDecodeDirect(const uint8_t* data, size_t len) {
  uint8_t tmpBuf[128];
  size_t i = 0;
  while (i < len) {
    // Fill tmpBuf with decoded pixels
    size_t tmpIdx = 0;
    while (i < len && tmpIdx < sizeof(tmpBuf)) {
      switch (imgRLEPhase) {
        case 0:
          imgRLECount = data[i++];
          imgRLEPhase = 1;
          break;
        case 1:
          imgRLEValue = data[i++];
          imgRLEPhase = 2;
          break;
        case 2: {
          size_t space = sizeof(tmpBuf) - tmpIdx;
          size_t n = imgRLECount;
          if (n > space) n = space;
          memset(tmpBuf + tmpIdx, imgRLEValue, n);
          tmpIdx += n;
          imgRLECount -= n;
          if (imgRLECount == 0) imgRLEPhase = 0;
          break;
        }
      }
    }
    // Stream decoded pixels to SPI
    if (tmpIdx > 0) {
      if (imgBpp == 8) {
        gloScreen->streamPixelsRGB332(tmpBuf, tmpIdx);
      } else if (imgBpp == 16) {
        // 16bpp RLE expands to raw bytes — need byte-pair handling
        for (size_t j = 0; j < tmpIdx; j++) {
          if (imgDirectHasCarry) {
            uint16_t pixel = imgDirectCarry | ((uint16_t)tmpBuf[j] << 8);
            gloScreen->streamPixels(&pixel, 1);
            imgDirectHasCarry = false;
          } else {
            imgDirectCarry = tmpBuf[j];
            imgDirectHasCarry = true;
          }
        }
      }
      imgPixelBufIndex += tmpIdx;
    }
  }
}

// Payload byte handler — delegates to block handler (handles RLE and all modes).
static void binImagePayloadByte(uint8_t byte) {
  binImagePayloadBlock(&byte, 1);
}

// Block handler — copies payload data in bulk using memcpy (buffer mode),
// writes into sprite buffer (sprite mode), or streams to SPI (direct mode)
static void binImagePayloadBlock(const uint8_t* data, size_t len) {
  // Consume sub-header bytes first
  while (imgSubHeaderIndex < IMAGE_SUBHEADER_SIZE && len > 0) {
    imgSubHeader[imgSubHeaderIndex++] = *data++;
    len--;
    if (imgSubHeaderIndex == IMAGE_SUBHEADER_SIZE) {
      binImageBegin();
    }
  }

  // If binImageBegin() rejected the frame (called binSendResponse with ok=false),
  // binFrameFailed is set.  The common parser will stop calling us for remaining
  // payload bytes.  For bytes already in this batch, return early.
  if (binFrameFailed || len == 0) return;

  if (imgMode == IMG_FLAG_SPRITE) {
    // Sprite mode — buffer into imgPixelBuf (copied into sprite during render)
    if (imgRLE) {
      binImageRLEDecode(data, len);
    } else if (imgPixelBuf) {
      size_t space = imgPixelBufSize - imgPixelBufIndex;
      size_t n = (len < space) ? len : space;
      memcpy(imgPixelBuf + imgPixelBufIndex, data, n);
      imgPixelBufIndex += n;
    }
  } else if (imgMode == IMG_FLAG_DIRECT) {
    // Direct mode — stream pixels to SPI as they arrive, or buffer if deferred
    if (!imgDirectActive) {
      // Deferred direct mode — buffer bytes (same as buffer mode)
      if (imgRLE) {
        binImageRLEDecode(data, len);
      } else {
        size_t remaining = (imgPixelsExpected * ((imgBpp + 7) / 8)) - imgPixelBufIndex;
        size_t n = (len < remaining) ? len : remaining;
        if (imgPixelBuf) {
          size_t space = imgPixelBufSize - imgPixelBufIndex;
          size_t copy = (n < space) ? n : space;
          memcpy(imgPixelBuf + imgPixelBufIndex, data, copy);
        }
        imgPixelBufIndex += n;
      }
      return;
    }
    // Active direct mode — stream to SPI
    if (imgRLE) {
      binImageRLEDecodeDirect(data, len);
    } else {
      size_t remaining = (imgPixelsExpected * ((imgBpp + 7) / 8)) - imgPixelBufIndex;
      size_t n = (len < remaining) ? len : remaining;
      if (imgBpp == 16) {
        const uint8_t* p = data;
        size_t left = n;
        // Handle carry byte from previous block
        if (imgDirectHasCarry && left > 0) {
          uint16_t pixel = imgDirectCarry | ((uint16_t)*p << 8);
          gloScreen->streamPixels(&pixel, 1);
          p++;
          left--;
          imgDirectHasCarry = false;
        }
        // Stream aligned pairs
        if (left >= 2) {
          size_t pairs = left / 2;
          uint16_t pixBuf[128];
          while (pairs > 0) {
            size_t batch = (pairs < 128) ? pairs : 128;
            memcpy(pixBuf, p, batch * 2);
            gloScreen->streamPixels(pixBuf, batch);
            p += batch * 2;
            pairs -= batch;
          }
          left = left % 2;
        }
        // Save odd trailing byte for next block
        if (left == 1) {
          imgDirectCarry = *p;
          imgDirectHasCarry = true;
        }
      } else if (imgBpp == 8) {
        gloScreen->streamPixelsRGB332(data, n);
      }
      imgPixelBufIndex += n;
    }
  } else {
    // Buffer mode (default) — bulk copy pixel data
    if (imgRLE) {
      binImageRLEDecode(data, len);
    } else if (imgPixelBuf) {
      size_t space = imgPixelBufSize - imgPixelBufIndex;
      size_t n = (len < space) ? len : space;
      memcpy(imgPixelBuf + imgPixelBufIndex, data, n);
      imgPixelBufIndex += n;
    }
  }
}

static uint8_t binImageGetCSPin(uint8_t port) {
  switch (port) {
    case 1: return DISPLAY_CS_1;
    case 2: return DISPLAY_CS_2;
    case 3: return DISPLAY_CS_3;
    default: return DISPLAY_CS_1;
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

  // Validate — on failure, binSendResponse() sets binFrameFailed; common parser
  // drains remaining payload and sends the deferred response after checksum.
  if (imgPort < 1 || imgPort > 3) {
    ESP_LOGW(TAG, "Image: invalid port %u", imgPort);
    binSendResponse(BIN_CMD_IMAGE, "invalid port", false);

    return;
  }
  if (imgBpp != 8 && imgBpp != 16) {
    ESP_LOGW(TAG, "Image: unsupported bpp %u", imgBpp);
    binSendResponse(BIN_CMD_IMAGE, "unsupported bpp", false);

    return;
  }
  if (imgWidth == 0 || imgHeight == 0 || imgWidth > 240 || imgHeight > 240) {
    ESP_LOGW(TAG, "Image: invalid dimensions %ux%u", imgWidth, imgHeight);
    binSendResponse(BIN_CMD_IMAGE, "invalid dimensions", false);

    return;
  }
  if (imgRLE) {
    // RLE: compressed payload is smaller than raw pixels — just validate minimum
    if (binPayloadLen < IMAGE_SUBHEADER_SIZE) {
      binSendResponse(BIN_CMD_IMAGE, "payload too short for RLE", false);
  
      return;
    }
  } else if (expectedPayload != binPayloadLen) {
    ESP_LOGW(TAG, "Image: payload length mismatch (expected %u, got %u)", expectedPayload, binPayloadLen);
    binSendResponse(BIN_CMD_IMAGE, "payload length mismatch", false);

    return;
  }
  if (!gloScreen) {
    ESP_LOGE(TAG, "Image: screen not initialized");
    binSendResponse(BIN_CMD_IMAGE, "screen not initialized", false);

    return;
  }

  // Mode-specific validation and setup
  if (imgMode == IMG_FLAG_SPRITE) {
    // Sprite mode: 8bpp only, must fit viewport
    if (imgBpp != 8) {
      binSendResponse(BIN_CMD_IMAGE, "sprite mode requires 8bpp", false);
  
      return;
    }
    if (imgWidth > VIEWPORT_W || imgHeight > VIEWPORT_H) {
      binSendResponse(BIN_CMD_IMAGE, "exceeds viewport", false);
  
      return;
    }
    // Allocate/reuse pixel buffer (same pattern as buffer mode)
    // Pixels are buffered here, then copied into sprite during render (inside semaphore)
    // to avoid race with DefaultView's chrome rendering on the shared sprite.
    if (imgPixelBuf && imgPixelBufSize == expectedDataBytes) {
      imgPixelBufIndex = 0;
    } else {
      free(imgPixelBuf);
      imgPixelBuf = nullptr;
      imgPixelBufSize = expectedDataBytes;
      imgPixelBufIndex = 0;
      imgPixelBuf = (uint8_t*)malloc(imgPixelBufSize);
    }
    if (!imgPixelBuf) {
      binSendResponse(BIN_CMD_IMAGE, "out of memory", false);
  
      return;
    }
    imageMode[imgPort - 1] = true;
    screenLockTime[imgPort - 1] = millis();
    ESP_LOGI(TAG, "Image: sprite mode port=%u %ux%u (%u bytes)",
             imgPort, imgWidth, imgHeight, imgPixelBufSize);

  } else if (imgMode == IMG_FLAG_DIRECT) {
    // Direct mode: try to acquire semaphore non-blocking to start SPI streaming.
    // If semaphore isn't available (DefaultView is rendering), fall back to
    // buffered mode — blocking here would stall the byte drain loop and cause
    // USB data loss (StreamBuffer overflow → binary parser timeout).
    if (imgWidth > VIEWPORT_W || imgHeight > VIEWPORT_H) {
      binSendResponse(BIN_CMD_IMAGE, "exceeds viewport", false);
  
      return;
    }
    if (xSemaphoreTake(screen_Semaphore, 0) == pdTRUE) {
      // Got semaphore — true direct SPI streaming
      imgDirectSemHeld = true;
      imageMode[imgPort - 1] = true;
      screenLockTime[imgPort - 1] = millis();
      imgDirectCSPin = binImageGetCSPin(imgPort);
      gloScreen->streamBegin(imgDirectCSPin, VIEWPORT_X, VIEWPORT_Y, imgWidth, imgHeight);
      imgDirectActive = true;
      ESP_LOGI(TAG, "Image: direct mode port=%u bpp=%u %ux%u", imgPort, imgBpp, imgWidth, imgHeight);
    } else {
      // Semaphore busy — fall back to buffered direct mode
      imgDirectActive = false;
      imgDirectSemHeld = false;
      // Allocate buffer (same as buffer mode)
      if (imgPixelBuf && imgPixelBufSize == expectedDataBytes) {
        imgPixelBufIndex = 0;
      } else {
        free(imgPixelBuf);
        imgPixelBuf = nullptr;
        imgPixelBufSize = expectedDataBytes;
        imgPixelBufIndex = 0;
        imgPixelBuf = (uint8_t*)malloc(imgPixelBufSize);
      }
      if (!imgPixelBuf) {
        binSendResponse(BIN_CMD_IMAGE, "out of memory", false);
    
        return;
      }
      imageMode[imgPort - 1] = true;
      screenLockTime[imgPort - 1] = millis();
      ESP_LOGI(TAG, "Image: direct-deferred mode port=%u bpp=%u %ux%u (sem busy)",
               imgPort, imgBpp, imgWidth, imgHeight);
    }

  } else {
    // Buffer mode (default) — allocate/reuse pixel buffer
    if (imgPixelBuf && imgPixelBufSize == expectedDataBytes) {
      imgPixelBufIndex = 0;  // reuse existing buffer
    } else {
      free(imgPixelBuf);
      imgPixelBuf = nullptr;
      imgPixelBufSize = expectedDataBytes;
      imgPixelBufIndex = 0;
      imgPixelBuf = (uint8_t*)malloc(imgPixelBufSize);
    }
    if (!imgPixelBuf) {
      static char oomMsg[120];
      snprintf(oomMsg, sizeof(oomMsg), "out of memory (need %u, free %u, largest %u)",
               (unsigned)imgPixelBufSize, (unsigned)esp_get_free_heap_size(),
               (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
      ESP_LOGE(TAG, "Image: %s", oomMsg);
      binSendResponse(BIN_CMD_IMAGE, oomMsg, false);
  
      return;
    }
    ESP_LOGI(TAG, "Image: buffer mode port=%u bpp=%u %ux%u (%u bytes)",
             imgPort, imgBpp, imgWidth, imgHeight, imgPixelBufSize);
  }
}

// Called after full frame received and CRC verified — does the actual work.
static void binImageRender() {
  if (!gloScreen) {
    binImageCleanup();
    return;
  }

  unsigned long tPreSem = micros();

  if (imgMode == IMG_FLAG_SPRITE) {
    // Sprite mode — copy buffered pixels into sprite, then push viewport.
    // All sprite writes happen inside semaphore to avoid race with DefaultView.
    if (!imgPixelBuf) {
      binImageCleanup();
      return;
    }
    if (xSemaphoreTake(screen_Semaphore, pdMS_TO_TICKS(200)) != pdTRUE) {
      ESP_LOGW(TAG, "Image: screen busy");
      binSendResponse(BIN_CMD_IMAGE, "screen busy", false);
      binImageCleanup();
      return;
    }

    imgProf.tSemAcq = micros();
    imageMode[imgPort - 1] = true;
    screenLockTime[imgPort - 1] = millis();

    // Copy pixel data into sprite at viewport offset
    uint8_t* spritePtr = (uint8_t*)gloScreen->img.getPointer();
    if (spritePtr) {
      for (uint16_t row = 0; row < imgHeight; row++) {
        size_t srcOff = row * imgWidth;
        size_t dstOff = (VIEWPORT_Y + row) * 240 + VIEWPORT_X;
        size_t copyLen = (srcOff + imgWidth <= imgPixelBufIndex) ? imgWidth : (imgPixelBufIndex > srcOff ? imgPixelBufIndex - srcOff : 0);
        if (copyLen > 0) {
          memcpy(spritePtr + dstOff, imgPixelBuf + srcOff, copyLen);
        }
      }
    }

    // Select target display CS pin
    uint8_t cs_pin = binImageGetCSPin(imgPort);
    digitalWrite(DISPLAY_CS_1, HIGH);
    digitalWrite(DISPLAY_CS_2, HIGH);
    digitalWrite(DISPLAY_CS_3, HIGH);
    digitalWrite(cs_pin, LOW);

    // Push viewport sub-region from sprite to display (8bpp→RGB565 via palette)
    gloScreen->img.pushSprite(VIEWPORT_X, VIEWPORT_Y,
                              VIEWPORT_X, VIEWPORT_Y, imgWidth, imgHeight);

    digitalWrite(cs_pin, HIGH);
    xSemaphoreGive(screen_Semaphore);

    imgProf.tSpiDone = micros();
    imgProf.active = false;

    binSendResponse(BIN_CMD_IMAGE, "sprite complete");
    binImageCleanup();
    return;
  }

  if (imgMode == IMG_FLAG_DIRECT) {
    if (imgDirectActive) {
      // True direct mode — SPI streaming already happened during payload.
      gloScreen->streamEnd();
      imgDirectActive = false;
      if (imgDirectSemHeld) {
        xSemaphoreGive(screen_Semaphore);
        imgDirectSemHeld = false;
      }
      screenLockTime[imgPort - 1] = millis();

      imgProf.tSemAcq = micros();
      imgProf.tSpiDone = micros();
      imgProf.active = false;

      binSendResponse(BIN_CMD_IMAGE, "direct complete");
      binImageCleanup();
      return;
    }

    // Deferred direct mode — pixels buffered because semaphore was busy during
    // payload receive. Now stream them to SPI (blocking semaphore OK here since
    // all bytes are already received).
    if (!imgPixelBuf) {
      binImageCleanup();
      return;
    }
    if (xSemaphoreTake(screen_Semaphore, pdMS_TO_TICKS(200)) != pdTRUE) {
      ESP_LOGW(TAG, "Image: screen busy (deferred direct)");
      binSendResponse(BIN_CMD_IMAGE, "screen busy", false);
      binImageCleanup();
      return;
    }

    imgProf.tSemAcq = micros();
    screenLockTime[imgPort - 1] = millis();

    uint8_t cs_pin = binImageGetCSPin(imgPort);
    gloScreen->streamBegin(cs_pin, VIEWPORT_X, VIEWPORT_Y, imgWidth, imgHeight);

    if (imgBpp == 16) {
      uint32_t pixelCount = imgPixelBufIndex / 2;
      gloScreen->streamPixels((const uint16_t*)imgPixelBuf, pixelCount);
    } else if (imgBpp == 8) {
      uint16_t pixBuf[128];
      size_t pixCount = 0;
      for (size_t i = 0; i < imgPixelBufIndex; i++) {
        pixBuf[pixCount++] = gloScreen->palette[imgPixelBuf[i]];
        if (pixCount >= 128) {
          gloScreen->streamPixels(pixBuf, pixCount);
          pixCount = 0;
        }
      }
      if (pixCount > 0) {
        gloScreen->streamPixels(pixBuf, pixCount);
      }
    }

    gloScreen->streamEnd();
    xSemaphoreGive(screen_Semaphore);

    imgProf.tSpiDone = micros();
    imgProf.active = false;

    binSendResponse(BIN_CMD_IMAGE, "direct complete");
    binImageCleanup();
    return;
  }

  // Buffer mode (default) — transfer buffered pixels to display via SPI
  if (!imgPixelBuf) {
    binImageCleanup();
    return;
  }

  // Acquire screen semaphore — block up to 200ms
  if (xSemaphoreTake(screen_Semaphore, pdMS_TO_TICKS(200)) != pdTRUE) {
    ESP_LOGW(TAG, "Image: screen busy");
    binSendResponse(BIN_CMD_IMAGE, "screen busy", false);
    binImageCleanup();
    return;
  }

  imgProf.tSemAcq = micros();

  // Mark this channel as image mode so render loop skips it
  imageMode[imgPort - 1] = true;
  screenLockTime[imgPort - 1] = millis();

  uint8_t cs_pin = binImageGetCSPin(imgPort);
  gloScreen->streamBegin(cs_pin, VIEWPORT_X, VIEWPORT_Y, imgWidth, imgHeight);

  // Stream buffered pixel data to display
  if (imgBpp == 16) {
    uint32_t pixelCount = imgPixelBufIndex / 2;
    gloScreen->streamPixels((const uint16_t*)imgPixelBuf, pixelCount);
  } else if (imgBpp == 8) {
    uint16_t pixBuf[128];
    size_t pixCount = 0;
    for (size_t i = 0; i < imgPixelBufIndex; i++) {
      pixBuf[pixCount++] = gloScreen->palette[imgPixelBuf[i]];
      if (pixCount >= 128) {
        gloScreen->streamPixels(pixBuf, pixCount);
        pixCount = 0;
      }
    }
    if (pixCount > 0) {
      gloScreen->streamPixels(pixBuf, pixCount);
    }
  }

  gloScreen->streamEnd();
  xSemaphoreGive(screen_Semaphore);

  imgProf.tSpiDone = micros();
  imgProf.active = false;

  // Send profiling data in the response
  unsigned long total = imgProf.tSpiDone - imgProf.t0;
  unsigned long usb = imgProf.tReceived - imgProf.t0;
  unsigned long crc = imgProf.tCrcDone - imgProf.tReceived;
  unsigned long sem = imgProf.tSemAcq - tPreSem;
  unsigned long spi = imgProf.tSpiDone - imgProf.tSemAcq;

  static char buf[256];
  snprintf(buf, sizeof(buf),
    "{\"status\":\"ok\",\"data\":{\"cmd\":1,\"message\":\"image complete\","
    "\"prof\":{\"total_us\":%lu,\"usb_us\":%lu,\"crc_us\":%lu,"
    "\"sem_us\":%lu,\"spi_us\":%lu,"
    "\"reads\":%u,\"bytes\":%u,\"wakes\":%u,\"avg_read\":%u}}}",
    total, usb, crc, sem, spi,
    imgProf.drainReads, imgProf.drainBytes, imgProf.drainWakes,
    imgProf.drainReads ? imgProf.drainBytes / imgProf.drainReads : 0);
  usbSerial.println(buf);
  usbSerial.flush();

  binImageCleanup();
}

static void binImageCleanup() {
  // Keep buffer allocated for reuse (avoids heap fragmentation on rapid frames)
  imgPixelBufIndex = 0;
}

static void binImageFreeBuffer() {
  // Keep the buffer allocated — freeing and re-mallocing 40KB causes
  // heap fragmentation on the ESP32-S3, leading to OOM on reconnect.
  // The buffer is reused if the next image has the same dimensions.
  imgPixelBufIndex = 0;
}

// --- Echo command (cmd=0x0002) ---
// Echoes payload back as a binary frame with the same cmd.
// Useful for round-trip latency testing and protocol verification.

static void binEchoBegin(uint32_t payloadLen) {
  if (payloadLen > 4096) {
    binSendResponse(BIN_CMD_ECHO, "echo payload too large", false);
    echoBuffer = nullptr;
    return;
  }
  if (payloadLen == 0) {
    echoBuffer = nullptr;
    echoBufferSize = 0;
    echoBufferIndex = 0;
    return;
  }
  echoBuffer = (uint8_t*)malloc(payloadLen);
  if (!echoBuffer) {
    binSendResponse(BIN_CMD_ECHO, "out of memory", false);
    return;
  }
  echoBufferSize = payloadLen;
  echoBufferIndex = 0;
}

static void binEchoPayloadByte(uint8_t byte) {
  if (!echoBuffer) return;
  if (echoBufferIndex < echoBufferSize) {
    echoBuffer[echoBufferIndex++] = byte;
  }
}

static void binEchoEnd() {
  // Send directly — safe because we're in task context now
  binSendBinaryFrame(BIN_CMD_ECHO, echoBuffer, echoBufferIndex);
  free(echoBuffer);
  echoBuffer = nullptr;
  echoBufferSize = 0;
  echoBufferIndex = 0;
}

// --- Meter stream command (cmd=0x0003) ---
// Subscribe payload: channel_mask(1) + interval_ms(2,LE)
// channel_mask bits 0-2 = CH1-CH3.  mask=0 stops streaming.
// Firmware sends unsolicited binary frames with meter samples.
//
// Sample frame payload: timestamp_ms(4,LE) + num_channels(1)
//   + [channel(1) + voltage_mV(4,float,LE) + current_mA(4,float,LE)] * num_channels

static void binMeterStreamPayloadByte(uint8_t byte) {
  if (meterSubPayloadIndex < sizeof(meterSubPayload)) {
    meterSubPayload[meterSubPayloadIndex++] = byte;
  }
}

static void binMeterStreamDispatch() {
  if (meterSubPayloadIndex < 3) {
    binSendResponse(BIN_CMD_METER_STREAM, "payload too short", false);
    return;
  }

  uint8_t mask = meterSubPayload[0];
  uint16_t interval = meterSubPayload[1] | (meterSubPayload[2] << 8);

  if (mask == 0) {
    // Stop streaming
    meterStreamActive = false;
    meterStreamChannelMask = 0;
    ESP_LOGI(TAG, "Meter stream: stopped");
    binSendResponse(BIN_CMD_METER_STREAM, "stopped");
    return;
  }

  if (mask > 0x07) {
    binSendResponse(BIN_CMD_METER_STREAM, "invalid channel mask", false);
    return;
  }
  if (interval < 20) interval = 20;    // cap at 50Hz
  if (interval > 10000) interval = 10000;

  meterStreamChannelMask = mask;
  meterStreamIntervalMs = interval;
  meterStreamLastSend = millis();
  meterStreamActive = true;

  ESP_LOGI(TAG, "Meter stream: mask=0x%02X interval=%ums", mask, interval);

  char msg[48];
  snprintf(msg, sizeof(msg), "streaming mask=0x%02X interval=%ums", mask, interval);
  binSendResponse(BIN_CMD_METER_STREAM, msg);
}

static void binSendBinaryFrame(uint16_t cmd, const uint8_t* payload, size_t len) {
  uint8_t header[9];
  header[0] = BIN_PROTOCOL_VERSION;
  header[1] = cmd & 0xFF;
  header[2] = (cmd >> 8) & 0xFF;
  header[3] = 0;  // flags low
  header[4] = 0;  // flags high
  header[5] = len & 0xFF;
  header[6] = (len >> 8) & 0xFF;
  header[7] = (len >> 16) & 0xFF;
  header[8] = (len >> 24) & 0xFF;

  uint32_t crc = esp_crc32_le(0, header, 9);
  if (len > 0) {
    crc = esp_crc32_le(crc, payload, len);
  }

  uint8_t escape = BIN_ESCAPE;
  usbSerial.write(&escape, 1);
  usbSerial.write(header, 9);
  if (len > 0) {
    usbSerial.write(payload, len);
  }
  uint8_t crcBytes[4] = {
    (uint8_t)(crc & 0xFF),
    (uint8_t)((crc >> 8) & 0xFF),
    (uint8_t)((crc >> 16) & 0xFF),
    (uint8_t)((crc >> 24) & 0xFF)
  };
  usbSerial.write(crcBytes, 4);
  usbSerial.flush();
}

static void binMeterStreamSendSample() {
  if (!meterStreamActive || !gloState) return;

  // Count active channels
  uint8_t numCh = 0;
  for (int i = 0; i < 3; i++) {
    if (meterStreamChannelMask & (1 << i)) numCh++;
  }

  // Build payload: timestamp(4) + num_channels(1) + [ch(1) + voltage(4) + current(4)] * n
  const size_t sampleSize = 1 + 4 + 4;  // channel + voltage_f32 + current_f32
  const size_t payloadLen = 4 + 1 + numCh * sampleSize;
  uint8_t buf[4 + 1 + 3 * 9];  // max 32 bytes for 3 channels

  uint32_t ts = millis();
  buf[0] = ts & 0xFF;
  buf[1] = (ts >> 8) & 0xFF;
  buf[2] = (ts >> 16) & 0xFF;
  buf[3] = (ts >> 24) & 0xFF;
  buf[4] = numCh;

  size_t offset = 5;
  for (int i = 0; i < 3; i++) {
    if (!(meterStreamChannelMask & (1 << i))) continue;
    buf[offset++] = i + 1;  // 1-indexed channel number
    float v = gloState->meter[i].AvgVoltage;
    float c = gloState->meter[i].AvgCurrent;
    memcpy(buf + offset, &v, 4); offset += 4;
    memcpy(buf + offset, &c, 4); offset += 4;
  }

  binSendBinaryFrame(BIN_CMD_METER_STREAM, buf, payloadLen);
}


//void onSerialDataReceived(const uint8_t* data, size_t length){
void onSerialDataReceived(){
  // Update binary timeout whenever we receive data in a binary state
  if (parseState != PARSE_TEXT) {
    binLastByteTime = millis();
  }
  // Process each byte
  for (size_t i = 0; i < rawBufIndex; i++) {
    uint8_t c = (uint8_t)rawBuffer[i];

    switch (parseState) {
    case PARSE_TEXT:
      if (c == BIN_ESCAPE) {
        // SOH — switch to binary header mode
        parseState = PARSE_BIN_HEADER;
        binHeaderIndex = 0;
        binRunningCRC = 0;
        binLastByteTime = millis();
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
          binFlushPendingResponse();
          break;
        }
        if (binPayloadLen > BIN_MAX_PAYLOAD) {
          ESP_LOGW(TAG, "Binary: payload too large %u", binPayloadLen);
          binSendResponse(binCmd, "payload too large", false);
          binReset();
          binFlushPendingResponse();
          break;
        }

        binPayloadReceived = 0;

        // Look up handler for this command
        binActiveHandler = binFindHandler(binCmd);
        if (!binActiveHandler) {
          binSendResponse(binCmd, "unknown command", false);
          // Continue to drain payload+checksum so they don't corrupt text stream
          if (binPayloadLen == 0) {
            parseState = PARSE_BIN_CHECKSUM;
            binChecksumIndex = 0;
          } else {
            parseState = PARSE_BIN_PAYLOAD;
          }
          break;
        }
        if (!binActiveHandler->begin(binCmd, binFlags, binPayloadLen)) {
          // Handler rejected — it sent its own error response.
          // Null out handler so payload bytes are drained silently,
          // and continue to consume payload+checksum so they don't
          // get misinterpreted as JSON text.
          binActiveHandler = nullptr;
          if (binPayloadLen == 0) {
            parseState = PARSE_BIN_CHECKSUM;
            binChecksumIndex = 0;
          } else {
            parseState = PARSE_BIN_PAYLOAD;
          }
          break;
        }

        if (binPayloadLen == 0) {
          // No payload — go straight to checksum
          parseState = PARSE_BIN_CHECKSUM;
          binChecksumIndex = 0;
        } else {
          parseState = PARSE_BIN_PAYLOAD;
        }
      }
      break;

    case PARSE_BIN_PAYLOAD: {
      // Process remaining payload bytes from rawBuffer as a block
      uint32_t remaining = binPayloadLen - binPayloadReceived;
      size_t available = rawBufIndex - i;
      size_t chunk = (available < remaining) ? available : remaining;

      // CRC over the block
      binRunningCRC = esp_crc32_le(binRunningCRC, (const uint8_t*)&rawBuffer[i], chunk);

      // Forward payload to handler — unless the frame has been marked as failed
      // (handler called binSendResponse with ok=false) or handler is null
      // (unknown command or begin() returned false).  In those cases, bytes are
      // silently drained; the deferred error response is sent after checksum.
      if (binActiveHandler && !binFrameFailed) {
        if (binActiveHandler->payloadBlock) {
          binActiveHandler->payloadBlock((const uint8_t*)&rawBuffer[i], chunk);
        } else {
          // Fallback to per-byte for handlers without block support
          for (size_t j = 0; j < chunk; j++) {
            binActiveHandler->payloadByte((uint8_t)rawBuffer[i + j]);
          }
        }
      }

      binPayloadReceived += chunk;
      i += chunk - 1;  // -1 because the for loop increments i

      if (binPayloadReceived >= binPayloadLen) {
        if (imgProf.active) imgProf.tReceived = micros();
        parseState = PARSE_BIN_CHECKSUM;
        binChecksumIndex = 0;
      }
      break;
    }

    case PARSE_BIN_CHECKSUM:
      binChecksumBuf[binChecksumIndex++] = c;
      if (binChecksumIndex == BIN_CHECKSUM_SIZE) {
        if (imgProf.active) imgProf.tCrcDone = micros();
        uint32_t expected = binChecksumBuf[0] | (binChecksumBuf[1] << 8) |
                           (binChecksumBuf[2] << 16) | (binChecksumBuf[3] << 24);
        if (expected != binRunningCRC) {
          ESP_LOGW(TAG, "Binary: checksum mismatch (expected 0x%08X, got 0x%08X)",
                   expected, binRunningCRC);
          binSendResponse(binCmd, "checksum mismatch", false);
        } else if (binFrameFailed) {
          // Handler already signaled failure — deferred error response is pending.
          // Skip dispatch; the response will be flushed below.
        } else if (binActiveHandler) {
          binActiveHandler->dispatch();
        }
        // Switch back to text mode BEFORE flushing response — this ensures
        // binSendResponse (called from dispatch or deferred) sees PARSE_TEXT
        // and sends immediately rather than deferring again.
        binReset();
        binFlushPendingResponse();
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
  
  if (!doc["action"]) {
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

    if(params.containsKey("screenLock")){
      JsonObject lp = params["screenLock"].as<JsonObject>();
      for (int i = 0; i < 3; i++) {
        String key = "CH" + String(i + 1);
        if (lp.containsKey(key)) {
          int val = lp[key].as<int>();
          if (val == 1) {
            imageMode[i] = true;
            screenLockTime[i] = millis();
          } else if (val == 0) {
            imageMode[i] = false;
            screenLockTime[i] = 0;
          } else {
            result[key] = "fail";
          }
        }
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

  else if(action == "get") {
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
      if(pName == "freeHeap"  || all || state) {
        result["freeHeap"]    = esp_get_free_heap_size();
        result["largestBlock"] = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
      }
      if(pName == "screenLock" || all || state) {
        JsonObject lo = result["screenLock"].to<JsonObject>();
        for (int i = 0; i < 3; i++)
          lo["CH" + String(i + 1)] = imageMode[i] ? 1 : 0;
      }

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

  else if(action == "bootloader"){
    if(gloConfig->features.reboot_enabled != ENABLE){
      result["error"] = "reboot_enabled must be set to 1 first";
      sendJsonResponse(0, result);
      return;
    }
    sendJsonResponse(0, result);
    usbSerial.flush();
    delay(100);
    enterBootloader();
  }

  else {
    String err = "{\"status\": \"error\", \"data\": {\"code\": -32601, \"message\": \"Unknown action: " + action + "\"}}";
    printErr(err);
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
        // With __wrap_tud_cdc_rx_cb, this event only fires if StreamBuffer
        // isn't ready yet (early boot fallback). Notify task as before.
        USBSerialActivity = true;
        if (exterTaskHandle) xTaskNotifyGive(exterTaskHandle);
        break;
      case ARDUINO_USB_CDC_RX_OVERFLOW_EVENT:
        ESP_LOGW(TAG,"CDC RX Overflow of %d bytes", data->rx_overflow.dropped_bytes);
        break;
     
      default:
        break;
    }
  }
}
