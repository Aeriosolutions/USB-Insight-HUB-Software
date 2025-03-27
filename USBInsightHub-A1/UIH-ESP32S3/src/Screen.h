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

//Screen class definitions and data structures for default view

#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>
#include <SPI.h>
#include "TFT_eSPI.h" // Hardware-specific library
//imported fonts to improve display quality
#include "modenine50.h"
#include "aptossb52l.h"
#include "aptossb30l.h"
#include "icons.h"
#include "datatypes.h"
#include "esp_clk.h"



#define DISPLAY_CS_1 6
#define DISPLAY_CS_2 16
#define DISPLAY_CS_3 12
#define DLIT_1 13
#define DLIT_2 15
#define DLIT_3 17
#define DISPLAY_ALL_DRES 14

#define PWN_RESOLUTION 10

#define BACKLIGHT_FREQ_240 290 //Hz. Higher frequencies cause flickering when WiFi is active
#define BACKLIGHT_FREQ_160 193

#define DARKGREY 0x7BF2

#define SMALLFONT aptossb30l

struct displayProp{
  uint8_t cs_pin;
  uint8_t dl_pin;
  uint8_t rotation;
  uint16_t brightness;
};

struct meterProp {
  float AvgVoltage;
  float AvgCurrent;
  float fwdCLim;
  float backCLim;
  bool fwdAlertSet;
  bool backAlertSet;
};

struct txtProp{
  int numDev;
  String Dev1_Name;
  String Dev2_Name;
  int usbType;
};

struct chScreenData { 
  displayProp dProp;
  meterProp mProp;
  txtProp tProp;
  bool fault;
  bool data_en;
  bool pwr_en;
  uint8_t ilim;
  bool pconnected;
  int startup_timer;
  int startup_cnt;
  uint8_t rssiBars;
  uint8_t wifiState;
  uint8_t hubMode;
  bool showMenuInfoSplash;
};


class Screen{
  public:
    //void start(TFT_eSPI *r_tft, TFT_eSprite *r_img);
    void start();
    void screenDefaultRender(chScreenData Screen);
    void screenSetBackLight(int pwm);
    void usbIconDraw(uint8_t type, bool active,bool com);

    displayProp dProp[3];
    TFT_eSPI tft       = TFT_eSPI();       // Invoke custom library
    TFT_eSprite img    = TFT_eSprite(&tft);

  private:    

    TFT_eSprite pcimg  = TFT_eSprite(&tft);
    TFT_eSprite wifiimg  = TFT_eSprite(&tft);
    TFT_eSprite udata = TFT_eSprite(&tft);

    uint16_t* imgPtr;
    uint16_t palette[256];
    uint16_t RGB332_to_RGB565(uint8_t rgb332);
    void setCSPins(uint8_t state);
};

#endif //SCREEN_H