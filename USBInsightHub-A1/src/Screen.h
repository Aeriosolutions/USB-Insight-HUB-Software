//Add licence text
//Screen class defiitions and data structures

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


#define DISPLAY_CS_1 6
#define DISPLAY_CS_2 16
#define DISPLAY_CS_3 12
#define DLIT_1 13
#define DLIT_2 15
#define DLIT_3 17
#define DISPLAY_ALL_DRES 14

#define PWN_RESOLUTION 10
#define BACKLIGHT_FREQ 290 //Hz. Higher frequencies cause flickering when WiFi is active


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
};


class Screen{
  public:
    //void start(TFT_eSPI *r_tft, TFT_eSprite *r_img);
    void start();
    void updateDefaultScreen(chScreenData Screen);
    
  private:
  
    TFT_eSPI tft       = TFT_eSPI();       // Invoke custom library
    TFT_eSprite img    = TFT_eSprite(&tft);
    TFT_eSprite pcimg  = TFT_eSprite(&tft);
    TFT_eSprite warimg = TFT_eSprite(&tft);

};

#endif //SCREEN_H