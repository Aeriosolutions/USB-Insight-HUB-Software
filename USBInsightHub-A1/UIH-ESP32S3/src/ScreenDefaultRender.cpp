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

//Default view rendering logic

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
  if(Screen.dProp.cs_pin == DISPLAY_CS_1 && Screen.usbHostState == USB_PLUGGED){
    if(Screen.pconnected){
      pcimg.pushImage(0, 0, 33, 29, PC);
    } else {
      pcimg.pushImage(0, 0, 33, 29, NOPC);
    }
    pcimg.pushToSprite(&img, 65, 0, TFT_WHITE);
  }

  //Startup timer indicator
  if(Screen.dProp.cs_pin == DISPLAY_CS_2 && Screen.startUpmode != PERSISTANCE){
    switch (Screen.startUpmode){      
      case START_ON:    ibuff.pushImage(0, 0, 33, 30, ONSTART_ON);        break;
      case START_OFF:   ibuff.pushImage(0, 0, 33, 30, ONSTART_OFF);       break;
      case STARTUP_SEC: ibuff.pushImage(0, 0, 33, 30, ONSTART_TIMER);     break;
    }
    ibuff.pushToSprite(&img, 75, 0, TFT_BLACK);    
  }

  //AUX power indicator
  if(Screen.dProp.cs_pin == DISPLAY_CS_2 && Screen.pwr_source == VEXT){
    ibuff.pushImage(0, 0, 33, 30, AUXPWR);
    ibuff.pushToSprite(&img, 120, 0, TFT_BLACK);
  }


  //connection icon
  if(Screen.dProp.cs_pin == DISPLAY_CS_3){

    if(Screen.wifiState == WIFI_OFFLINE) ibuff.pushImage(0, 0, 33, 30, WIFIE);
    else if(Screen.wifiState == STA_CONNECTED || Screen.wifiState == STA_NOCLIENT){
      if(Screen.rssiBars == 0) ibuff.pushImage(0, 0, 33, 30, WIFI0_);
      else if(Screen.rssiBars == 1) ibuff.pushImage(0, 0, 33, 30, WIFI1_);
      else if(Screen.rssiBars == 2) ibuff.pushImage(0, 0, 33, 30, WIFI2_);
      else if(Screen.rssiBars == 3) ibuff.pushImage(0, 0, 33, 30, WIFI3_);
      else ibuff.pushImage(0, 0, 33, 30, WIFIE);
    }
    else if(Screen.wifiState == AP_CONNECTED  || Screen.wifiState == AP_NOCLIENT){
      if(Screen.rssiBars == 0) ibuff.pushImage(0, 0, 33, 30, AP0);
      else if(Screen.rssiBars == 1) ibuff.pushImage(0, 0, 33, 30, AP1);
      else if(Screen.rssiBars == 2) ibuff.pushImage(0, 0, 33, 30, AP2);            
      else if(Screen.rssiBars == 3 || Screen.wifiState == AP_NOCLIENT) ibuff.pushImage(0, 0, 33, 30, AP3);
    }
    else if(Screen.wifiState == WIFI_OFF)
      ibuff.pushImage(0, 0, 33, 30, NOWIFI);

    int dx = 115;
    ibuff.pushToSprite(&img, dx, 0, TFT_BLACK);
   
    if(Screen.wifiState == STA_CONNECTED){
      img.fillTriangle(dx+26,24,dx+32,30,dx+32,18,TFT_WHITE);
    }
    if(Screen.wifiState == AP_CONNECTED){
      img.fillTriangle(dx+10,30,dx+16,23,dx+22,30,TFT_WHITE);
    }
     
  }  

  //data switch indicator
  if(Screen.hubMode == USB2) usbIconDraw(3,false,false);
  if(Screen.hubMode == USB2_3 || Screen.hubMode == USB3){
    Screen.tProp.usbType == 3 && Screen.pconnected ? usbIconDraw(3,true,true) : usbIconDraw(3,true,false);    
  }   
  
  if(Screen.hubMode == USB3) usbIconDraw(2,false,false);  
  if(Screen.hubMode == USB2_3 || Screen.hubMode == USB2)
  {
    if(!Screen.data_en){
      //img.setTextColor(TFT_RED);
      //img.drawRightString("DATA", 238, 2, 4);
      //img.fillRect(165, 10, 100, 2, TFT_RED) ;
      usbIconDraw(2,false,false);      
    }
    else{
      //img.setTextColor(TFT_YELLOW);
      //img.drawRightString("DATA", 238, 2, 4);
      Screen.tProp.usbType == 2 && Screen.pconnected ? usbIconDraw(2,true,true) : usbIconDraw(2,true,false);                  
    }
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

  //Screen.tProp.numDev=10;

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


  if(Screen.tProp.numDev == 10) flexDevicePrint(Screen.tProp.Dev1_Name,Screen.pconnected);

  
  //ESP_LOGI("3","%u",millis()-timers); //----------------------------------------

  //USB type info
  img.setTextSize(1);
  img.setTextColor(TFT_WHITE);
  
  int32_t tiw = 40;
  int32_t tit = 20;
  
  if(Screen.tProp.numDev == 10) {tiw = 20; tit = 10;}

  if(Screen.tProp.usbType == 2) {    
    img.fillRoundRect(0, 33, tiw, 22, 5, TFT_RED);
    img.drawCentreString(Screen.tProp.numDev==10 ? "2":"2.0", tit, 32, 4);
  }

  if(Screen.tProp.usbType == 3) {
    img.fillRoundRect(0, 33, tiw, 22, 5, TFT_BLUE); 
    img.drawCentreString(Screen.tProp.numDev==10 ? "3":"3.0", tit, 32, 4);
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

  //Menu access information splash

  if(Screen.dProp.cs_pin == DISPLAY_CS_3 && Screen.showMenuInfoSplash){
    img.loadFont(SMALLFONT);
    img.fillRoundRect(5, 40, 235, 80, 5, TFT_BLUE);
    img.fillRoundRect(9, 44, 229, 76, 5, TFT_WHITE);
    img.setTextColor(TFT_BLACK);
    img.drawString("Long press to",10,50,4);
    img.drawString("enter Setup",10,80,4);
    img.fillTriangle(204,45,204,75,230,60,TFT_BLACK);
    img.unloadFont();
  }

  /*
  //Use for palette evaluation
  int offset = 0;
  if(Screen.dProp.cs_pin == DISPLAY_CS_2) offset = 40; 

  img.fillScreen(TFT_BLACK);
  img.fillRoundRect(offset, 0, 200, 25, 1, TFT_WHITE);
  img.fillRoundRect(offset, 25, 200, 25, 1, TFT_YELLOW);
  img.fillRoundRect(offset, 50, 200, 25, 1, TFT_GREEN);
  img.fillRoundRect(offset, 75, 200, 25, 1, TFT_ORANGE);
  img.fillRoundRect(offset, 100, 200, 25, 1, TFT_RED);
  img.fillRoundRect(offset, 125, 200, 25, 1, TFT_CYAN);
  img.fillRoundRect(offset, 150, 200, 25, 1, TFT_BLUE);
  img.fillRoundRect(offset, 175, 200, 25, 1, TFT_LIGHTGREY);
  img.fillRoundRect(offset, 200, 200, 25, 1, DARKGREY);
  */  


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

//------------------------------ HELPERS -------------------------------------

void Screen::usbIconDraw(uint8_t type, bool active, bool com){

  uint32_t x, y, w, h, color;

  if (type != 2 && type != 3) return;

  udata.fillScreen(TFT_BLACK);

  if(type == 2){
    udata.pushImage(0, 0, 12, 19, NARROW2);
    if(active){
      x=168; y=1; w=34; h=28; color=TFT_RED;
    }
    else{      
      x=168; y=1; w=34; h=28; color=DARKGREY;
    }
  }
  if(type == 3){
    udata.pushImage(0, 0, 12, 19, NARROW3);
    if(active){
      x=206; y=1; w=34; h=28; color=TFT_BLUE;
    }
    else{
      x=206; y=1; w=34; h=28; color=DARKGREY;
    }
  }

  img.fillRoundRect(x, y, w, h, 3, color);

  udata.pushToSprite(&img, x+20, 5, TFT_BLACK);
  
  udata.fillScreen(TFT_BLACK);
  if(active){
    if(com){
      udata.pushImage(0, 0, 19, 21, COMARROW);
      udata.pushToSprite(&img, x+1, 5, TFT_BLACK);
    } else {
      udata.pushImage(0, 0, 11, 26, CSWITCH);
      udata.pushToSprite(&img, x+3, 1, TFT_BLACK);      
    }

  } else {
    udata.pushImage(0, 0, 11, 26, NOSWITCH);
    udata.pushToSprite(&img, x+3, 1, TFT_BLACK);
  }

}

//This function is used to print variable length texts sent by the 
//Enumeration extraction agent via serial
void Screen::flexDevicePrint(String jsonStr, bool pcCon){

  JsonDocument doc;

  
  u_int16_t color = TFT_WHITE;
  int ty[3]= {70,0,0};

  int lines = 1;

  if(jsonStr != "")
  {
    DeserializationError error = deserializeJson(doc, jsonStr);  
    if(!error){
      img.loadFont(monofonto30);
      //vertical text offsets by number of lines      
      if(doc["T1"] && doc["T2"] && doc["T3"]) {ty[0] = 42; ty[1] = 72; ty[2] = 102; lines = 3;}   //three lines
      if(doc["T1"] && doc["T2"] && !doc["T3"]) {ty[0] = 55; ty[1] = 85; lines = 2;}  //two lines
      if(doc["T1"] && !doc["T2"] && !doc["T3"]) {ty[0] = 70; lines = 1;} //one line

      for (int i=0; i< lines; i++){

        String ind = "T"+String(i+1);

        if(doc[ind]["color"] == "YELLOW") color = TFT_YELLOW;
        else if(doc[ind]["color"] == "ORANGE")  color = TFT_ORANGE;
        else if(doc[ind]["color"] == "BLACK")  color = TFT_BLACK;
        else if(doc[ind]["color"] == "RED")  color = TFT_RED;
        else if(doc[ind]["color"] == "DARKGREY")  color = DARKGREY;
        else if(doc[ind]["color"] == "CYAN")  color = TFT_CYAN;
        else if(doc[ind]["color"] == "BLUE")  color = TFT_BLUE;
        else if(doc[ind]["color"] == "GREEN")  color = TFT_GREEN;

        img.setTextColor(color);

        if (!pcCon) img.setTextColor(TFT_LIGHTGREY);

        String temp = String(doc[ind]["txt"].as<const char*>());
        temp = temp.substring(0,14);

        if(doc[ind]["align"] == "left"){
          img.drawString(temp,15,ty[i],4);
        } 
        else if(doc[ind]["align"] == "right"){
          img.drawRightString(temp, 230, ty[i], 4);
        } 
        else if(doc[ind]["txt"]){
          img.drawCentreString(temp, 120, ty[i], 4);
        }
      }  
      img.unloadFont();        
    }
  }

}