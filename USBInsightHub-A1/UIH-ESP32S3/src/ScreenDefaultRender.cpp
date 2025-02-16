#include "Screen.h"

void Screen::screenDefaultRender(chScreenData Screen){
  int cval = 0;
  String aux = "";
  long cbarmax = 2000;
  String device = "*";
  uint32_t color;
  int faultType = 0;
  
  unsigned long timers=0;
  unsigned long timere=0;
  //Fault classification
  timers=millis();

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
  
  //PC image
  
  if(Screen.dProp.cs_pin == DISPLAY_CS_1){
    if(Screen.pconnected){
      pcimg.pushImage(0, 0, 33, 29, PC);
    } else {
      pcimg.pushImage(0, 0, 33, 29, NOPC);
    }
    pcimg.pushToSprite(&img, 65, 0, TFT_WHITE);
  }
  
  //connection icon
  if(Screen.dProp.cs_pin == DISPLAY_CS_3){

    if(Screen.wifiState == WIFI_OFFLINE) wifiimg.pushImage(0, 0, 33, 30, WIFIE);
    else if(Screen.wifiState == STA_CONNECTED || Screen.wifiState == STA_NOCLIENT){
      if(Screen.rssiBars == 0) wifiimg.pushImage(0, 0, 33, 30, WIFI0_);
      else if(Screen.rssiBars == 1) wifiimg.pushImage(0, 0, 33, 30, WIFI1_);
      else if(Screen.rssiBars == 2) wifiimg.pushImage(0, 0, 33, 30, WIFI2_);
      else if(Screen.rssiBars == 3) wifiimg.pushImage(0, 0, 33, 30, WIFI3_);
      else wifiimg.pushImage(0, 0, 33, 30, WIFIE);
    }
    else if(Screen.wifiState == AP_CONNECTED  || Screen.wifiState == AP_NOCLIENT){
      if(Screen.rssiBars == 0) wifiimg.pushImage(0, 0, 33, 30, AP0);
      else if(Screen.rssiBars == 1) wifiimg.pushImage(0, 0, 33, 30, AP1);
      else if(Screen.rssiBars == 2) wifiimg.pushImage(0, 0, 33, 30, AP2);            
      else if(Screen.rssiBars == 3 || Screen.wifiState == AP_NOCLIENT) wifiimg.pushImage(0, 0, 33, 30, AP3);
    }

    if(Screen.wifiState != WIFI_OFF)
      wifiimg.pushToSprite(&img, 110, 0, TFT_BLACK);
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

  //ESP_LOGI("1","%u",millis()-timers); //----------------------------------------
  
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

  //ESP_LOGI("2","%u",millis()-timers); //----------------------------------------

  img.loadFont(SMALLFONT);

  //if(Screen.fault) img.drawCentreString("S",20,180,4);

  //Ilim print
  img.setTextSize(1);
  img.setTextColor(TFT_LIGHTGREY);

  aux = String(Screen.mProp.fwdCLim/1000,1);
  aux.concat("A");
  if(Screen.mProp.fwdCLim != 0)
    cbarmax = (long)(Screen.mProp.fwdCLim);
  else
    cbarmax = 1000;
 
  //current limit value
  img.drawString(aux, 2, 220, 4);

  //current bar
  cval = (cval * 150) / cbarmax;
  // HWSerial.println(cval);
  img.fillRect(65, 222, cval, 18, TFT_CYAN) ;

  //Device name box
  img.fillRoundRect(7, 40, 226, 90, 10, DARKGREY); //**
  
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

  //ESP_LOGI("3","%u",millis()-timers); //----------------------------------------

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



  //startup counter
  if(Screen.startup_cnt > 0){
    img.loadFont(aptossb52l);  
    img.setTextSize(2);
    img.setTextColor(TFT_GREEN);
    img.fillRoundRect(7, 40, 226, 90, 10, TFT_BLACK);
    cval = ((Screen.startup_timer-Screen.startup_cnt) * 226) / Screen.startup_timer;
    img.fillRoundRect(7, 40, cval, 90, 10, DARKGREY);
    //aux = String((Screen.startup_cnt+9)/10) + "/" + String((Screen.startup_timer+10)/10);
    aux = String((float)(Screen.startup_cnt)/10) + "s";
    img.drawCentreString(aux, 120, 65, 4); //**
    img.unloadFont();
  }


 
  //ESP_LOGI("4","%u",millis()-timers); //---------------------------------------- 
  

  //timers=millis();
  img.pushSprite(0, 0);
  //tft.pushImageDMA(0,0,240,240,imgPtr); // use only with 16bit color depth
  //timere=millis();
  //ESP_LOGI("P","%u",millis()-timers); //----------------------------------------- 
  
  /*
  while(tft.dmaBusy()){
    vTaskDelay(pdMS_TO_TICKS(5));    
  }*/
  
  //ESP_LOGI("W","%u",millis()-timers); //---------------------------------------- 

  digitalWrite(Screen.dProp.cs_pin, HIGH);
  
}