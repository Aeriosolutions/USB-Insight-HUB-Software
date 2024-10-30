//Add licence text
//Screen class function and draw logic

//TFT_eSPI version: 2.5.43
//esp32 arduino version: 2.0.14

#include "Screen.h"

#define SMALLFONT aptossb30l

void Screen::start(){

  pinMode(DISPLAY_CS_1, OUTPUT);
  pinMode(DISPLAY_CS_2, OUTPUT);
  pinMode(DISPLAY_CS_3, OUTPUT);
  pinMode(DISPLAY_ALL_DRES, OUTPUT);

  //tft = TFT_eSPI();       // Invoke custom library
  

  //PWM signals for backlight control
  //On Espressif library 2.0.14
  analogWriteResolution(10);
  analogWriteFrequency(5000);

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

void Screen::updateScreen(chScreenData Screen){
  int cval = 0;
  String aux = "";
  long cbarmax = 2000;
  String device = "*";
  uint32_t color;
  int faultType = 0;

  //Fault classification

  if(Screen.pwr_en && Screen.fault) faultType = 1;
  else if (Screen.mProp.fwdAlertSet||Screen.mProp.backAlertSet) faultType = 2;
  else faultType = 0;
  img.loadFont(SMALLFONT);
  //analogWrite(Screen.dProp.dl_pin,Screen.dProp.brightness);

  digitalWrite(Screen.dProp.cs_pin, LOW);
  tft.setRotation(Screen.dProp.rotation);  
  img.fillScreen(TFT_BLACK);
  img.setTextColor(TFT_WHITE);  
  img.setTextFont(1);  

  //power indicator
  img.setTextSize(1); 

  if(!Screen.pwr_en){
    color = faultType == 2 ? TFT_YELLOW : TFT_LIGHTGREY; 
    img.setTextColor(color);
    //img.fillRect(60, 0, 45, 20, TFT_RED) ;
    img.drawString("OFF", 2, 2, 4);
    img.fillRoundRect(0, 33, 240, 104, 10, color);
  }
  else{
    //Screen.fault == true ? color=TFT_RED:color=TFT_GREEN;
    switch(faultType){
      case 0: color = TFT_GREEN;  break;
      case 1: color = TFT_RED;    break;
      case 2: color = TFT_YELLOW; break;
      default : break;
    } 
    img.setTextColor(color);
    //img.fillRect(60,0,45,20,color);
    img.drawString("ON", 2, 2, 4);
    img.fillRoundRect(0, 33, 240, 104, 10, color);
  }
  

  //data switch indicator
  if(!Screen.data_en){
    img.setTextColor(TFT_RED);
    img.drawRightString("DATA", 238, 2, 4);
    img.fillRect(165, 10, 100, 2, TFT_RED) ;
  }
  else{
    img.setTextColor(TFT_YELLOW);
    img.drawRightString("DATA", 238, 2, 4);
  }  
  
  img.unloadFont();
  
  img.loadFont(aptossb52l);
  //voltage print
  img.setTextSize(2);
  img.setTextColor(TFT_GREEN);

  aux = String(Screen.mProp.AvgVoltage/1000, 3);
  //aux = String(Screen.mProp.AvgVoltage,0);

  aux.concat(" V");
  img.drawRightString(aux, 235, 142, 4);

  //current print
  img.setTextSize(2);
  switch(faultType){
    case 0: color = TFT_CYAN;   break;
    case 1: color = TFT_RED;    break;
    case 2: color = TFT_YELLOW; break;
    default : break;
  }   
  img.setTextColor(color);
  aux = String(Screen.mProp.AvgCurrent/1000,3);  

  //to avoid "dancing" negative sign
  if (Screen.mProp.AvgCurrent <= 0.2 && Screen.mProp.AvgCurrent > -0.2){
    aux= "0.000";
  } 
  //  cval= aux.toInt();
  cval = int(Screen.mProp.AvgCurrent);
  aux.concat(" A");
  img.drawRightString(aux, 235, 182, 4);
  img.unloadFont();

  img.loadFont(SMALLFONT);

  //if(Screen.fault) img.drawCentreString("S",20,180,4);

  //Ilim print
  img.setTextSize(1);
  img.setTextColor(TFT_LIGHTGREY);

  switch(Screen.ilim+1)
  {
    case 1: 
      aux = "0.5A";
      cbarmax = 500;
      break;
    case 2: 
      aux = "1.0A";
      cbarmax = 1000;
      break;
    case 3: 
      aux = "1.5A";
      cbarmax = 1500;
      break;
    case 4: 
      aux = "2.0A";
      cbarmax = 2000;
      break;
    default:
      aux = "?.?A";
      cbarmax = 2000;
      break;
  }

  //current limit value
  img.drawString(aux, 2, 220, 4);

  //current bar
  cval = (cval * 150) / cbarmax;
  // HWSerial.println(cval);
  img.fillRect(65, 222, cval, 18, TFT_CYAN) ;

  //Device name box
  img.fillRoundRect(7, 40, 226, 90, 10, 0x7BF2); //**
  
  //Device text print
  //img.setTextSize(2);
  img.unloadFont();
  img.loadFont(modenine50); 

  if(Screen.tProp.numDev==0){
    img.setTextColor(TFT_LIGHTGREY);
    device = "----";
    img.drawCentreString(device, 120, 65, 4); //**
  } 
  else if(Screen.tProp.numDev == 1){
    Screen.pconnected == true ? img.setTextColor(TFT_YELLOW) : img.setTextColor(TFT_LIGHTGREY);
    //img.setTextColor(TFT_YELLOW);
    device = Screen.tProp.Dev1_Name;
    img.drawCentreString(device, 120, 65, 4); //**
  } 
  else if(Screen.tProp.numDev == 2){
    Screen.pconnected == true ? img.setTextColor(TFT_YELLOW) : img.setTextColor(TFT_LIGHTGREY);
    //img.setTextColor(TFT_YELLOW);   
    img.drawCentreString(Screen.tProp.Dev1_Name,120,45,4);
    Screen.pconnected == true ? img.setTextColor(TFT_WHITE) : img.setTextColor(TFT_LIGHTGREY);
    //img.setTextColor(TFT_ORANGE);
    img.drawCentreString(Screen.tProp.Dev2_Name,120,85,4); //**
  }

  img.unloadFont();

  //img.loadFont(SMALLFONT);
  //USB type info
  img.setTextSize(1);
  img.setTextColor(TFT_WHITE);
  if(Screen.tProp.usbType == 2) {
    //img.fillRoundRect(0,31,42,26,5,TFT_RED); //If SMALLFONT IS USED    
    img.fillRoundRect(0, 33, 40, 22, 5, TFT_RED);
    img.drawCentreString("2.0", 20, 32, 4);
  }

  if(Screen.tProp.usbType == 3) {
    img.fillRoundRect(0, 33, 40, 22, 5, TFT_BLUE); 
    img.drawCentreString("3.0", 20, 32, 4);
  }

  //Fault indicator
  int font=2;
  int center=175;
  switch(faultType)
  {
    case 1: 
      color = TFT_RED;
      aux   = "!";
      break;
    case 2: 
      color  = TFT_YELLOW;
      font   = 1;
      center = 185;
      aux    = Screen.mProp.fwdAlertSet == true ? "OC" : "BC"; 
      break;
    default:
	  break;
  } 

  if(faultType != 0)
  {
    //warimg.pushImage(0,0,35,35,OCWARNING);
    //warimg.pushToSprite(&img,2,180,TFT_WHITE);
    //img.loadFont(modenine50);     
    img.fillRoundRect(5, 175, 40, 40, 10, color);
    img.setTextColor(TFT_BLACK);
    img.setTextSize(font);
    img.drawCentreString(aux, 25, center, 4);
  }

  //PC image
  
  if(Screen.dProp.cs_pin == DISPLAY_CS_1){
    if(Screen.pconnected){
      pcimg.pushImage(0, 0, 33, 29, PC);
    } else {
      pcimg.pushImage(0, 0, 33, 29, NOPC);
    }
    pcimg.pushToSprite(&img, 65, 0, TFT_WHITE);
  }

  /*
  //test counter
  img.setTextSize(1);
  img.setTextColor(TFT_LIGHTGREY);
  img.drawCentreString(String(Screen.dProp.brightness),140,2,4);  
  */
  //img.unloadFont();

  img.pushSprite(0, 0);
  digitalWrite(Screen.dProp.cs_pin, HIGH);
  
}