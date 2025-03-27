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

 //definition of global variables and data structures

#ifndef DATATYPES_H
#define DATATYPES_H

#include <Arduino.h>
#include <Wire.h>
#include "BaseMCU.h"
#include "PAC194x.h"
#include "Screen.h"

#define DATATYPES_VER 4

#define APP_CORE 1

#define DISPLAY_REFRESH_PERIOD    63 //note that for each screen the effective rate is 189ms
#define SLOW_DATA_DOWNSAMPLES_0_5  8 //504ms //multiples of DISPLAY_REFRESH_PERIOD 
#define SLOW_DATA_DOWNSAMPLES_1_0 16 //1008ms //multiples of DISPLAY_REFRESH_PERIOD 
//------------ Start Pin definitions ------------

//STEMMA
#define STEMMA_SCL  7
#define STEMMA_SDA  8

//I2C Pins
#define B2B_SCL    33
#define B2B_SDA    34

//Interrupt Pins
#define PAC_ALERT  35
#define MCU_INT    18

//Buttons
#define BUTTON_3   37
#define BUTTON_2   38
#define BUTTON_1   39
//#define SETUP      18 //For B0 hardware
#define SETUP      36 //For C0 hardware

//Other
#define VBUS_MONITOR   1
#define HUB_RESET     21
#define AUX_LED       45

//--------------End pin definitions -------------

//System->currentView;
#define DEFAULT_VIEW  1
#define SETTINGS_VIEW 2

//Feature State ->wifistate
#define WIFI_OFFLINE    0 //Device is completely offline
#define AP_NOCLIENT     1 //Access Point is available, but no client is connected
#define AP_CONNECTED    2 //Access Point is used and at least 1 client is connected
#define STA_NOCLIENT    3 //Device connected to a WiFi Station
#define STA_CONNECTED   4 //Device connected to a WiFi Station and at least 1 client is connected
#define STA_MQTT        5 //Device connected to a WiFi Station and the device is connected to a MQTT server
#define WIFI_OFF        6 //WIFI is off

static const char* t_wifiState[] = {"offline","ap_noclient","ap_connected","sta_noclient","sta_connected","sta_mqtt,off"};

//Features Config->starupMode
#define PERSISTANCE   0
#define START_ON      1
#define START_OFF     2
#define STARTUP_SEC   3

static const char* t_startupMode[] = {"persistance","on_at_start","off_at_start","sequence"};

//Features Config->hubMode
#define USB2_3  0
#define USB2    1
#define USB3    2

static const char* t_hubMode[] = {"usb2&3","usb2","usb3"};

#define DISABLE 0
#define ENABLE  1

static const char* t_bool[] = {"false","true"};

//Features Config->refreshRate
#define S0_5 0
#define S1_0 1

static const char* t_refreshRate[] = {"0.5s","1.0s"};

//Features Config->filterType
#define FILTER_MOVING_AVG 0
#define FILTER_MEDIAN 1

static const char* t_filterType[] = {"moving_avg", "median"};

//Screen Config Rotation
#define ROT_0_DEG     0
#define ROT_90_DEG    1
#define ROT_180_DEG   2
#define ROT_270_DEG   3

static const char* t_rotation[] = {"0","90","180","270"};

//State BaseMCUExtra vx_cc
#define UNKNOWN    0
#define DEFAULT    1
#define C_1_5A     2
#define C_3_0A     3

static const char* t_vx_cc[] = {"unknown", "default", "1.5A", "3.0A"};

//State BaseMCUExtra vx_stat
#define NO_PULLUPS    0
#define CC1_PULLUP    1
#define CC2_PULLUP    2
#define BOTH_PULLUPS  3

static const char* t_vx_stat[] = {"no_pullups","cc1_pu","cc2_pu","both"};

//State BaseMCUExtra pwr
#define VHOST   false
#define VEXT    true


struct System {
  uint8_t currentView;
  TaskHandle_t taskIntercommHandle;
  TaskHandle_t taskDefaultScreenLoopHandle;
  bool saveMCUState;
  String APSSID;
  bool congigChangedToMenu;
  bool configChangedFromMenu;
  bool firstStart;
  bool ledState;
  String wifiMAC;
  bool menuIsActive;
  bool showMenuInfoSplash;
  bool resetToDefault;  
};

struct FeaturesState {
  bool startUpActive;
  bool pcConnected;
  float vbus;
  String ssid;
  uint8_t wifiState;
  String wifiIP;
  String wifiAPIP;
  int8_t wifiRSSI;
  uint8_t wifiClients;
  uint8_t wifiRecovery;
  uint8_t wifiReset;      
};

struct FeaturesConfig {
  uint8_t startView; 
  uint8_t startUpmode;  
  uint8_t wifi_enabled;
  uint8_t hubMode;
  uint8_t filterType;    
  uint8_t refreshRate;
};

struct StartupState { 
  int startup_cnt;
};

struct StartupConfig {
  int startup_timer;
};

struct ScreenConfig {
  uint8_t rotation;
  uint16_t brightness;
};

struct MeterState {
  float AvgVoltage;
  float AvgCurrent;
  bool fwdAlertSet;
  bool backAlertSet;
};

struct MeterConfig {
  uint16_t fwdCLim;
  uint16_t backCLim;
};

struct USBInfoState {
  int numDev;
  String Dev1_Name;
  String Dev2_Name;
  int usbType;
};

struct BaseMCUStateIn {
  bool fault; 
};

struct BaseMCUStateOut {
  uint8_t ilim;
  bool data_en;
  bool pwr_en;  
};


struct BaseMCUExtraState { 
  uint8_t vext_cc;
  uint8_t vhost_cc;
  uint8_t vext_stat;
  uint8_t vhost_stat;
  bool pwr_source;
  bool usb3_mux_out_en;
  bool usb3_mux_sel_pos;
  uint8_t base_ver;
};

//all the volatile variables that change constantly during operation and do need persistance
struct GlobalState {
    System system;
    FeaturesState features;
    StartupState startup[3];
    MeterState meter[3];
    USBInfoState usbInfo[3];
    BaseMCUStateIn baseMCUIn[3];    
    BaseMCUStateOut baseMCUOut[3];    
    BaseMCUExtraState baseMCUExtra;
};

//all the configuration variables that change only with configuration changes or need persistance
struct GlobalConfig {
    FeaturesConfig features;
    StartupConfig startup[3];
    ScreenConfig screen[3];
    MeterConfig meter[3];
    //BaseMCUConfig baseMCU[3];
};

#endif // DATATYPES_H
