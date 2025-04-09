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

 //Logic for the default view (Devices metadata, voltage/current meter, etc.)

#include "DefaultView.h"

GlobalState *gState;
GlobalConfig *gConfig;
Screen *iScreen;

static const char* TAG = "DefaultView";

chScreenData ScreenArr[3];
chScreenData prevScreenArr[3];

txtProp prevDevInfo[3];

void defaultScreenFastDataUpdate();
void defaultScreenSlowDataUpdate();
uint8_t getRssiBars(int8_t rssi);

void taskDefaultViewLoop(void *pvParameters);
void taskDefaultScreenLoop(void *pvParameters);

bool defaultViewActive = false;
SemaphoreHandle_t screen_Semaphore;
unsigned long infoSplashTimer = 0;
uint8_t prevRefreshRate = 0;

void iniDefaultView(GlobalState* globalState, GlobalConfig* globalConfig,  Screen *screen){

  gState = globalState;
  gConfig = globalConfig;
  iScreen = screen;
  
  
  screen_Semaphore = xSemaphoreCreateMutex();

  ESP_LOGI(TAG,"Started on Core %u",xPortGetCoreID());

  iScreen->dProp[0] = {DISPLAY_CS_1, DLIT_1, ROT_180_DEG, 800};
  iScreen->dProp[1] = {DISPLAY_CS_2, DLIT_2, ROT_180_DEG, 800};
  iScreen->dProp[2] = {DISPLAY_CS_3, DLIT_3, ROT_180_DEG, 800};

  ScreenArr[0].dProp = {DISPLAY_CS_1, DLIT_1, ROT_180_DEG, 800};
  ScreenArr[1].dProp = {DISPLAY_CS_2, DLIT_2, ROT_180_DEG, 800};
  ScreenArr[2].dProp = {DISPLAY_CS_3, DLIT_3, ROT_180_DEG, 800};
  
  defaultScreenFastDataUpdate();
  prevRefreshRate = gConfig->features.refreshRate;

    
  if(xSemaphoreTake(screen_Semaphore,( TickType_t ) 20 ) == pdTRUE){
    
    iScreen->start();
    for (int i=0; i<3; i++) {
      iScreen->screenDefaultRender(ScreenArr[i]);
    }
    
    //iScreen->screenSetBackLight(gConfig->screen[0].brightness);

    xSemaphoreGive(screen_Semaphore);
    defaultViewStart();

  } 
  else {
    ESP_LOGE(TAG, "Screen resource taken, could not initialize");
  }

}

void defaultViewStart(void){
  if(!defaultViewActive){
    defaultViewActive = true;
    xTaskCreatePinnedToCore(taskDefaultScreenLoop, "Default Screen", 4096, NULL, 4, &(gState->system.taskDefaultScreenLoopHandle),APP_CORE);
    xTaskCreatePinnedToCore(taskDefaultViewLoop, "Default View", 2048, NULL, 3, NULL,APP_CORE);      
  }
}

void taskDefaultViewLoop(void *pvParameters){  
  ESP_LOGI(TAG,"Loop Logic on Core %u",xPortGetCoreID());
  for(;;){
    if(gState->system.currentView==DEFAULT_VIEW && defaultViewActive){
      int tot = 0;
      for (int i=0; i<3; i++){
        //tot=resolveButton(&ButtonArray[i]);
        if (btnShortCheck(i)) {
          ESP_LOGI(TAG,"Button %s short press",String(i+1));

          //If button pressed during startup timer, cancel the timer and leave off          
          if(gState->startup[i].startup_cnt>0){ 
            gState->startup[i].startup_cnt=-1;
            ESP_LOGI(TAG,"Cancel Start up timer");
          }
          else {
            //If OC or BC is set, clear the fault and leave off
            if(gState->meter[i].backAlertSet || gState->meter[i].fwdAlertSet){
              ESP_LOGI(TAG,"Reset OC and BC");
              gState->meter[i].backAlertSet = false;
              gState->meter[i].fwdAlertSet  = false;
              gState->baseMCUOut[i].pwr_en = false;
            }
            //Normal condition, toggle output ON/OFF
            else
            { 
              gState->baseMCUOut[i].pwr_en = !gState->baseMCUOut[i].pwr_en;              
            }
          }

        }
        if (btnLongCheck(i)) {
          ESP_LOGI(TAG,"Button %s long press",String(i+1));
          if(gConfig->features.hubMode == USB2_3 || gConfig->features.hubMode == USB2)
            gState->baseMCUOut[i].data_en = !gState->baseMCUOut[i].data_en;
        }
      }
      
      if (btnShortCheck(3)) {
        
        ESP_LOGI(TAG,"Setup button short press");
        uint16_t oclimit=2000;
        gState->system.showMenuInfoSplash = true;
        infoSplashTimer = millis();
        //fast current limit option
        if(USE_FAST_CURRENT_SETUP > 0) 
        {
          for (int i=0; i<3 ; i++){
            switch(gState->baseMCUOut[i].ilim){
                case 0:                 
                  oclimit=1000;
                  break;
                case 1:                 
                  oclimit=1500;
                  break;
                case 2:                 
                  oclimit=2000;
                  break;
                case 3:                 
                  oclimit=500;
                  break;              
                default:
                  break;
            }
            gConfig->meter[i].fwdCLim=oclimit;
            if(gState->meter[i].backAlertSet || gState->meter[i].fwdAlertSet){
              ESP_LOGI(TAG,"Reset OC and BC");
              gState->meter[i].backAlertSet = false;
              gState->meter[i].fwdAlertSet  = false;
              gState->baseMCUOut[i].pwr_en = false;
            }
          }          
        } 
      }
      if (btnLongCheck(3)){
        
        ESP_LOGI(TAG,"Setup button long press");

        defaultViewActive=false;


      }       
    }
    if(!defaultViewActive && gState->system.taskDefaultScreenLoopHandle == NULL)
    {
      menuViewStart(gState,gConfig,iScreen);
      ESP_LOGI(TAG,"Delete Default View Task Loop");
      vTaskDelete(NULL);
    }
    vTaskDelay(pdMS_TO_TICKS(DEFAULT_VIEW_PERIOD));
  }   
}

void taskDefaultScreenLoop(void *pvParameters){
  
  int iScnt = 0;
  int slowCnt = 0;
  int slowPeriod = 10;
  bool firstPass = false;
  uint16_t prevBrightness = 10; //this value is out of range 0-3 to force first update 
  unsigned long timers=0;
  unsigned long timere=0;
  
  TickType_t xLastWakeTime = xTaskGetTickCount();
  ESP_LOGI(TAG,"Loop Screen on Core %u",xPortGetCoreID());
  
  if(xSemaphoreTake(screen_Semaphore,( TickType_t ) 10 ) == pdTRUE)
  {
    for(;;){
      
      xTaskNotifyGive(gState->system.taskIntercommHandle);
      ulTaskNotifyTake(pdTRUE,pdMS_TO_TICKS(20));

      //adjust brightness only if there is a change to avoid flikering
      if(prevBrightness != gConfig->screen[0].brightness && firstPass){        
        iScreen->screenSetBackLight(gConfig->screen[0].brightness);
        prevBrightness = gConfig->screen[0].brightness;
      }
      //keep backlight off for the first update for the three screens
      if(!firstPass) iScreen->screenSetBackLight(0);

      defaultScreenFastDataUpdate();
      if(prevRefreshRate != gConfig->features.refreshRate){
        if(gConfig->features.refreshRate == S0_5) slowPeriod = SLOW_DATA_DOWNSAMPLES_0_5;
        if(gConfig->features.refreshRate == S1_0) slowPeriod = SLOW_DATA_DOWNSAMPLES_1_0;
        slowCnt=0;
        prevRefreshRate = gConfig->features.refreshRate;
      }
      slowCnt++;
      if(slowCnt == slowPeriod){
        slowCnt=0;
        defaultScreenSlowDataUpdate();      
      }

      //update screen only if something changed or on the first update cycle      
      if(memcmp(&prevScreenArr[iScnt],&(ScreenArr[iScnt]),sizeof(prevScreenArr[iScnt])) != 0 || !firstPass){
        timers=millis();
        iScreen->screenDefaultRender(ScreenArr[iScnt]);
        timere=millis();
        prevScreenArr[iScnt] = ScreenArr[iScnt];
        //ESP_LOGI(TAG,"%u",timere-timers);            
      }

      iScnt++;
      if(iScnt==3) {iScnt=0;  firstPass = true;  }

      if(infoSplashTimer != 0){
        if(millis()-infoSplashTimer > MENU_INFO_SPLASH_TIMEOUT){
          gState->system.showMenuInfoSplash = false;
          infoSplashTimer = 0;
        }
      }

      if(!defaultViewActive){
        ESP_LOGI(TAG,"Delete Screen Loop");
        xSemaphoreGive(screen_Semaphore);
        gState->system.taskDefaultScreenLoopHandle = NULL;
        vTaskDelete(NULL);
      }      

      vTaskDelayUntil(&xLastWakeTime,pdMS_TO_TICKS(DISPLAY_REFRESH_PERIOD));
    }
  }
  else {
    ESP_LOGE(TAG,"Screen resource bussy, can't create Screen Loop Task");
    vTaskDelete(NULL);
  }

}

void defaultScreenFastDataUpdate(){

    for (int i=0; i<3 ; i++){      
      ScreenArr[i].fault   = gState->baseMCUIn[i].fault;
      ScreenArr[i].data_en = gState->baseMCUOut[i].data_en;
      ScreenArr[i].pwr_en  = gState->baseMCUOut[i].pwr_en;
      ScreenArr[i].ilim    = gState->baseMCUOut[i].ilim;
      ScreenArr[i].mProp.fwdAlertSet  = gState->meter[i].fwdAlertSet;
      ScreenArr[i].mProp.backAlertSet = gState->meter[i].backAlertSet;
      ScreenArr[i].mProp.fwdCLim      = gConfig->meter[i].fwdCLim;
      ScreenArr[i].mProp.backCLim     = gConfig->meter[i].backCLim;
      ScreenArr[i].tProp.numDev     = gState->usbInfo[i].numDev;
      ScreenArr[i].tProp.Dev1_Name  = gState->usbInfo[i].Dev1_Name;
      ScreenArr[i].tProp.Dev2_Name  = gState->usbInfo[i].Dev2_Name;
      ScreenArr[i].tProp.usbType    = gState->usbInfo[i].usbType;      
      ScreenArr[i].pconnected       = gState->features.pcConnected;
      iScreen->dProp[i].brightness  = gConfig->screen[i].brightness;
      iScreen->dProp[i].rotation    = gConfig->screen[i].rotation;
      ScreenArr[i].dProp.brightness = gConfig->screen[i].brightness;
      ScreenArr[i].dProp.rotation   = gConfig->screen[i].rotation;
      ScreenArr[i].startup_cnt      = gState->startup[i].startup_cnt;
      ScreenArr[i].startup_timer    = gConfig->startup[i].startup_timer;
      ScreenArr[i].rssiBars         = getRssiBars(gState->features.wifiRSSI);
      ScreenArr[i].wifiState        = gState->features.wifiState;
      ScreenArr[i].hubMode          = gConfig->features.hubMode; 
      ScreenArr[i].showMenuInfoSplash   = gState->system.showMenuInfoSplash;
      ScreenArr[i].startUpmode      = gConfig->features.startUpmode;
      ScreenArr[i].pwr_source       = gState->baseMCUExtra.pwr_source; 
      ScreenArr[i].usbHostState     = gState->features.usbHostState;

      if( memcmp(&prevDevInfo[i],&(ScreenArr[i].tProp),sizeof(prevDevInfo[i])) != 0 ){
        ESP_LOGI(TAG, "CH %u: #d %u, D1: %s, D2: %s, t: %u",i,ScreenArr[i].tProp.numDev,
        gState->usbInfo[i].Dev1_Name,
        gState->usbInfo[i].Dev2_Name, 
        ScreenArr[i].tProp.usbType);    
        //save current state for later comparison    
        memcpy(&prevDevInfo[i],&(ScreenArr[i].tProp),sizeof(prevDevInfo[i]));
      }       
    }  
}

void defaultScreenSlowDataUpdate(){
    for (int i=0; i<3 ; i++){     
      ScreenArr[i].mProp.AvgCurrent = gState->meter[i].AvgCurrent;
      ScreenArr[i].mProp.AvgVoltage = gState->meter[i].AvgVoltage;
    }  
    //ESP_LOGV(TAG, "CH 0 state: %s, screen: %s",gState->usbInfo[0].Dev1_Name, ScreenArr[0].tProp.Dev1_Name);    
}

uint8_t getRssiBars(int8_t rssi){
  //copy values from RSSIIndicator.svelte
  if (rssi>= -55) return 3;
  else if (rssi>= -75) return 2;
  else if (rssi>= -85) return 1;
  else return 0;

}