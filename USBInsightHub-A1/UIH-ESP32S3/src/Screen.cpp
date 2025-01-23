//Add licence text
//Screen class function and draw logic

//TFT_eSPI version: 2.5.43
//esp32 arduino version: 2.0.14

#include "Screen.h"


void Screen::start(){

  pinMode(DISPLAY_CS_1, OUTPUT);
  pinMode(DISPLAY_CS_2, OUTPUT);
  pinMode(DISPLAY_CS_3, OUTPUT);
  pinMode(DISPLAY_ALL_DRES, OUTPUT);

  //tft = TFT_eSPI();       // Invoke custom library

  //PWM signals for backlight control
  //On Espressif library 2.0.14
  analogWriteResolution(PWN_RESOLUTION);
  analogWriteFrequency(BACKLIGHT_FREQ);

  analogWrite(DLIT_1, 0);
  analogWrite(DLIT_2, 0);
  analogWrite(DLIT_3, 0);
  delay(150); //100ms wait after power up before reset
  //Reset Displays
  digitalWrite(DISPLAY_ALL_DRES, HIGH);
  delay(10); //10ms reset pulse
  digitalWrite(DISPLAY_ALL_DRES, LOW);
  delay(10); //10ms wait before sending commands
  // We need to 'init' all displays
  // at the same time. so set both cs pins low
  digitalWrite(DISPLAY_CS_1, LOW);
  digitalWrite(DISPLAY_CS_2, LOW);
  digitalWrite(DISPLAY_CS_3, LOW);

  tft.init();
  tft.writecommand(ST7789_TEON); //Enable tearing effect signal
  tft.writedata(0x00);
  
  img.createSprite(240, 240);
  pcimg.createSprite(33, 29);
  pcimg.setSwapBytes(true);
  warimg.createSprite(35, 35);
  warimg.setSwapBytes(true);
  tft.setTextSize(2);
  tft.setRotation(2);
  tft.fillScreen(TFT_BLUE);
  
  // Set both cs pins HIGH, or 'inactive'
  digitalWrite(DISPLAY_CS_1, HIGH);
  digitalWrite(DISPLAY_CS_2, HIGH);
  digitalWrite(DISPLAY_CS_3, HIGH);

  //analogWrite(DLIT_1, 800);
  //analogWrite(DLIT_2, 800);
  //analogWrite(DLIT_3, 800);
}