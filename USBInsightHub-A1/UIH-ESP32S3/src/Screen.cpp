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


//Screen initialization and helpers

//TFT_eSPI version: 2.5.43
//esp32 arduino version: 2.0.14

#include "Screen.h"


void Screen::start(){

  pinMode(DISPLAY_CS_1, OUTPUT);
  pinMode(DISPLAY_CS_2, OUTPUT);
  pinMode(DISPLAY_CS_3, OUTPUT);
  pinMode(DISPLAY_ALL_DRES, OUTPUT);

  //If CS pins are not left low at initialization
  //the displays stay blank with soft resets 
  setCSPins(LOW); 
 
  //PWM signals for backlight control
  //On Espressif library 2.0.14
  analogWriteResolution(PWN_RESOLUTION);
  if (esp_clk_cpu_freq() == 240000000){ 
    analogWriteFrequency(BACKLIGHT_FREQ_240);
    ESP_LOGI("Screen","Backlight for 240MHZ set");
  }
  else{
    analogWriteFrequency(BACKLIGHT_FREQ_160);  
    ESP_LOGI("Screen","Backlight for 160MHZ set");
  }
    

  screenSetBackLight(0);
  delay(50); //50ms wait after power up before reset
  //Reset Displays
  digitalWrite(DISPLAY_ALL_DRES, HIGH);
  delay(10); //10ms reset pulse
  digitalWrite(DISPLAY_ALL_DRES, LOW);
  delay(10); //10ms wait before sending commands
  // We need to 'init' all displays
  // at the same time. so set all cs pins low
  setCSPins(LOW);


  tft.init();
  tft.initDMA();
  
  tft.writecommand(ST7789_TEON); //Enable tearing effect signal
  tft.writedata(0x00);

  if(ALT_DISPLAY_CONFIG > 0)
  {
    tft.writecommand(ST7789_PORCTRL); //PORCH Control 0xB2
    tft.writedata(0x1F);
    tft.writedata(0x1F);
    tft.writedata(0x00);
    tft.writedata(0x33);
    tft.writedata(0x33);
    
    tft.writecommand(ST7789_GCTRL); //GATE CONTROL Control 0xB7
    tft.writedata(0x00); //VGH=12.2V,VGL=-7.16V

    tft.writecommand(ST7789_VCOMS); //VCOM Settings 0xBB
    tft.writedata(0x3F); //1.675V

    //tft.writecommand(ST7789_LCMCTRL); //LCM CONTROL 0xC0
    //tft.writedata(0x2C); //S7W Reset library is 0xC

    tft.writecommand(ST7789_VRHS); //VRH Set 0xC3
    tft.writedata(0x0F); //4.3V

    //tft.writecommand(ST7789_VDVSET); //VDV Set 0xC4
    //tft.writedata(0x20); //0.0V - Same as driver

    tft.writecommand(ST7789_FRCTR2); //Frame Rate control in Normal Mode 0xC6
    tft.writedata(0x13); //53Hz

    tft.writecommand(ST7789_PWCTRL1);
    tft.writedata(0xa4);
    tft.writedata(0xa1);

    tft.writecommand(ST7789_PVGAMCTRL); //Positive Voltage Gamma Control 0xE0
    tft.writedata(0xF0);
    tft.writedata(0x06);
    tft.writedata(0x0D);
    tft.writedata(0x0B);
    tft.writedata(0x0A);
    tft.writedata(0x07);
    tft.writedata(0x2E);
    tft.writedata(0x43);
    tft.writedata(0x45);
    tft.writedata(0x38);
    tft.writedata(0x14);
    tft.writedata(0x13);
    tft.writedata(0x25);
    tft.writedata(0x29);

    tft.writecommand(ST7789_NVGAMCTRL); //Negative Voltage Gamma Control 0xE0
    tft.writedata(0xF0);
    tft.writedata(0x07);
    tft.writedata(0x0A);
    tft.writedata(0x08);
    tft.writedata(0x07);
    tft.writedata(0x23);
    tft.writedata(0x2E);
    tft.writedata(0x33);
    tft.writedata(0x44);
    tft.writedata(0x3A);
    tft.writedata(0x16);
    tft.writedata(0x17);
    tft.writedata(0x26);
    tft.writedata(0x2C);

    //GATECTRL is left untouched, use driver config
    tft.writecommand(ST7789_INVON); //Inversion ON 0x21
    tft.writecommand(ST7789_SLPOUT); //Sleep Out 0x11
    delay(120);
    tft.writecommand(ST7789_DISPON); //Display ON 0x29

  }

  // Create the RGB332 palette (256 colors)
  for (int i = 0; i < 256; i++) {
      palette[i] = RGB332_to_RGB565(i);
  }

  img.setColorDepth(8); //use 8 bit color depth to save 57.6kB of SRAM
  imgPtr = (uint16_t*)img.createSprite(240, 240); //imgPtr used for DMA transaction, only with 16bit color depth
  //img.createPalette(palette);
  
  pcimg.createSprite(33, 29);
  pcimg.setSwapBytes(true);
  ibuff.createSprite(33, 30);
  ibuff.setSwapBytes(true);
  udata.createSprite(32,28);
  udata.setSwapBytes(true);
  udata.fillScreen(TFT_BLACK);
  
  
  tft.setTextSize(2);
  tft.setRotation(2);
  tft.fillScreen(TFT_BLUE);
  
  // Set both cs pins HIGH, or 'inactive'
  setCSPins(HIGH);

}

void Screen::screenSetBackLight(int pwm){
  analogWrite(DLIT_1, pwm);
  analogWrite(DLIT_2, pwm);
  analogWrite(DLIT_3, pwm);  
}

uint16_t Screen::RGB332_to_RGB565(uint8_t rgb332) {
    uint8_t r = (rgb332 >> 5) & 0x07;  // Extract 3-bit Red
    uint8_t g = (rgb332 >> 2) & 0x07;  // Extract 3-bit Green
    uint8_t b = (rgb332 >> 0) & 0x03;  // Extract 2-bit Blue

    // Convert to full 8-bit values and then to RGB565 format
    r = (r * 255) / 7;  
    g = (g * 255) / 7;
    b = (b * 255) / 3;

    return tft.color565(r, g, b);
}

void Screen::setCSPins(uint8_t state){
  digitalWrite(DISPLAY_CS_1, state);
  digitalWrite(DISPLAY_CS_2, state);
  digitalWrite(DISPLAY_CS_3, state);
}