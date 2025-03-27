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

//Global variables initialization and non-volatile variables handling

#include "GlobalStateManager.h"

static const char* TAG = "GlobalStateManager";

Preferences flashstorage;

GlobalState *globlState;
GlobalConfig *globlConfig;

GlobalConfig prevGloblConfig;

void setDefaultGlobalConfig(GlobalState *globalState, GlobalConfig *globalConfig);


void globalStateInitializer(GlobalState *globalState, GlobalConfig *globalConfig){

    //check_reset_reason();
    

    globlState  = globalState;
    globlConfig = globalConfig;

    pinMode(AUX_LED,OUTPUT);

    //Load the Configuration from NVS system
    flashstorage.begin(UIH_NAMESPACE,false);
    if(flashstorage.getInt("ConfigInit")!=MEM_INITIALIZED_NUM){
        //flashstorage.end();
        ESP_LOGI(TAG,"NVM is not initialized...load default values under template: %u", DATATYPES_VER);
        setDefaultGlobalConfig(globalState,globalConfig);
        flashstorage.putInt("TemplateVer",DATATYPES_VER);
        flashstorage.putBytes("ConfigBlob",globalConfig,sizeof(*globalConfig));
        flashstorage.putBytes("MCUBlob",&(globalState->baseMCUOut),sizeof(globalState->baseMCUOut));
        flashstorage.putInt("ConfigInit",MEM_INITIALIZED_NUM);
        flashstorage.end();
        ESP_LOGI(TAG,"Saved new template to NVM");        
    } 
    else {
        int tver = flashstorage.getInt("TemplateVer");
        if(tver!=DATATYPES_VER){           
           ESP_LOGW(TAG,"Template in NVM is version %u. This version supports version %u", tver,DATATYPES_VER);
           setDefaultGlobalConfig(globalState,globalConfig);
           flashstorage.putInt("TemplateVer",DATATYPES_VER);
           //clear previous template keys
           flashstorage.remove("ConfigBlob");
           flashstorage.remove("MCUBlob");
           //create keys again with new lengths (if is the case) and default values
           flashstorage.putBytes("ConfigBlob",globalConfig,sizeof(*globalConfig));
           flashstorage.putBytes("MCUBlob",&(globalState->baseMCUOut),sizeof(globalState->baseMCUOut));
           flashstorage.end();            
           ESP_LOGI(TAG,"Default templete ver %d created",DATATYPES_VER); 
        }
        else {
            //Load configuration from NVM
            
            if(flashstorage.getBytes("ConfigBlob",globalConfig,sizeof(*globalConfig))==sizeof(*globalConfig)){
                ESP_LOGI(TAG,"Config loaded correctly");
            } else {
                ESP_LOGW(TAG,"Failed to load Config... upload defaults");
                setDefaultGlobalConfig(globalState,globalConfig);
            }

            if(flashstorage.getBytes("MCUBlob",&(globalState->baseMCUOut),sizeof(globalState->baseMCUOut))==sizeof(globalState->baseMCUOut)){
                ESP_LOGI(TAG,"MCU blob loaded correctly");
            } else {
                ESP_LOGW(TAG,"Failed to load MCU blob... upload defaults");
                for(int i =0; i<3; i++){
                    globalState->baseMCUOut[i].ilim = ILIM_1_0;
                    globalState->baseMCUOut[i].data_en = true;
                    globalState->baseMCUOut[i].pwr_en = true;  
                }              
            }
            flashstorage.end();
        }
    }
 

    //Hydrate Global State variables

    //---System

    globalState->system.currentView = globalConfig->features.startView;
    globalState->system.saveMCUState = false;
    globalState->system.APSSID = "";
    globalState->system.congigChangedToMenu = false;
    globalState->system.configChangedFromMenu = false;
    globalState->system.firstStart = true;
    globalState->system.ledState = false;
    String mac = WiFi.macAddress();
    mac.replace(":", "");
    globalState->system.wifiMAC = mac;
    globalState->system.menuIsActive = false;
    globalState->system.resetToDefault = false;

    //---Features
    globalConfig->features.startUpmode != STARTUP_SEC ? globalState->features.startUpActive = false : globalState->features.startUpActive = true;
    globalState->features.pcConnected   = false;
    globalState->features.vbus   = 5000; //mV Fixed value, Not implemented yet
    globalConfig->features.wifi_enabled == ENABLE ? globalState->features.wifiState = WIFI_OFFLINE : globalState->features.wifiState = WIFI_OFF;
    globalState->features.wifiState     = WIFI_OFF;
    globalState->features.wifiAPIP      = "192.168.4.1";
    globalState->features.wifiIP        = "0.0.0.0";
    globalState->features.wifiReset     = 0;
    globalState->features.wifiRecovery  = 0;

    
    for(int i=0; i<3; i++){
        //---Startup    
        globalState->startup[i].startup_cnt = 0;
        
        //---Meter
        globalState->meter[i].AvgVoltage = 0;
        globalState->meter[i].AvgCurrent = 0;
        globalState->meter[i].backAlertSet = false;
        globalState->meter[i].fwdAlertSet = false; 
        
        //---USB Info
        globalState->usbInfo[i].numDev = 0;
        globalState->usbInfo[i].Dev1_Name ="-";
        globalState->usbInfo[i].Dev2_Name ="-";
        globalState->usbInfo[i].usbType = 0;



        //---BaseMCU
        globalState->baseMCUIn[i].fault = false;
        switch(globalConfig->features.startUpmode){
            case PERSISTANCE:                
                break;
            case START_ON:                
                globalState->baseMCUOut[i].data_en = true;
                globalState->baseMCUOut[i].pwr_en = true;
                break;
            case START_OFF:                
                globalState->baseMCUOut[i].data_en = true;
                globalState->baseMCUOut[i].pwr_en = false;
                break;
            case STARTUP_SEC:
                globalState->startup[i].startup_cnt = globalConfig->startup[i].startup_timer;                
                globalState->baseMCUOut[i].data_en = true;
                globalState->baseMCUOut[i].pwr_en = false;
                break;
        }

        //---USB HUB Mode
        if(globalConfig->features.hubMode == USB3) globalState->baseMCUOut[i].data_en = false;

    }

    //---BaseMCUExtra
    globalState->baseMCUExtra.vext_cc = UNKNOWN;
    globalState->baseMCUExtra.vhost_cc = UNKNOWN;
    globalState->baseMCUExtra.vext_stat = NO_PULLUPS;
    globalState->baseMCUExtra.vhost_stat = NO_PULLUPS;
    globalState->baseMCUExtra.pwr_source = VHOST;  
    globalState->baseMCUExtra.usb3_mux_out_en = false;
    globalState->baseMCUExtra.usb3_mux_sel_pos = false;
    globalState->baseMCUExtra.base_ver = 255;

    //start task to detect
    prevGloblConfig = *globlConfig;
    xTaskCreatePinnedToCore(taskConfigAutoSave, "ConfigAutoSave", 2048, NULL, 5, NULL,APP_CORE);
}

void setDefaultGlobalConfig(GlobalState *globalState, GlobalConfig *globalConfig){
    
    globalConfig->features.startView = DEFAULT_VIEW;
    globalConfig->features.startUpmode = PERSISTANCE;    
    globalConfig->features.wifi_enabled = ENABLE;
    globalConfig->features.hubMode = USB2_3;
    globalConfig->features.filterType = FILTER_TYPE_MEDIAN;
    globalConfig->features.refreshRate = S0_5;

    for(int i=0; i<3; i++){
        globalConfig->startup[i].startup_timer = 1;

        globalConfig->screen[i].rotation = ROT_180_DEG;
        globalConfig->screen[i].brightness = 800;  

        globalConfig->meter[i].backCLim = 20; //mA
        globalConfig->meter[i].fwdCLim = 1000; //mA        

        globalState->baseMCUOut[i].ilim = ILIM_1_0;
        globalState->baseMCUOut[i].data_en = true;
        globalState->baseMCUOut[i].pwr_en = true;
        
    }
    //Defaults
    globalConfig->startup[0].startup_timer = 10;
    globalConfig->startup[1].startup_timer = 20;
    globalConfig->startup[2].startup_timer = 30;
}


void taskConfigAutoSave(void *pvParameters){
  TickType_t xLastWakeTime = xTaskGetTickCount();
  uint8_t dumb = 0;
  long index = 0;
  for(;;){
    //check if is a change in config parameters
    if( memcmp(&prevGloblConfig, globlConfig, sizeof(prevGloblConfig)) != 0 ){
        saveConfig();
        //discriminate if the configuration change comes from the Menu or elsewhere
        if(!globlState->system.configChangedFromMenu) globlState->system.congigChangedToMenu = true;
        globlState->system.configChangedFromMenu = false;

        if(globlConfig->features.wifi_enabled != prevGloblConfig.features.wifi_enabled){
            vTaskDelay(pdMS_TO_TICKS(90));
            ESP.restart();
        }
        memcpy(&prevGloblConfig, globlConfig, sizeof(prevGloblConfig));
    }
    if(globlState->system.saveMCUState){
        saveMCUState();
        globlState->system.saveMCUState = false;
    }

    //check if it is needed to reset to defaults
    if(globlState->system.resetToDefault){
        ESP_LOGI(TAG,"Command: Reset to default values");
        globlState->system.resetToDefault = false;
        setDefaultGlobalConfig(globlState,globlConfig);
    }
    
    //Uncomment to increase flash size by 1575783 bytes to test big file OTA
    /*
    dumb = blobdata[index];
    if(dumb == 0) ESP_LOGI(TAG,"Never value");     
    if (index < sizeof(blobdata)-1) index++;
    else index = 0;
    */      
    vTaskDelayUntil(&xLastWakeTime,pdMS_TO_TICKS(CONFIG_AUTO_SAVE_PERIOD));
  }
}

void saveMCUState(void){
    flashstorage.begin(UIH_NAMESPACE,false);
    flashstorage.putBytes("MCUBlob",&(globlState->baseMCUOut),sizeof(globlState->baseMCUOut));
    ESP_LOGI(TAG,"Save MCU blob");
    flashstorage.end();
}

void saveConfig(void){
    flashstorage.begin(UIH_NAMESPACE,false);
    flashstorage.putBytes("ConfigBlob",globlConfig,sizeof(*globlConfig));
    ESP_LOGI(TAG,"Save Config blob");
    flashstorage.end();
}

void check_reset_reason() {
    esp_reset_reason_t reason = esp_reset_reason();
    if (!(reason == ESP_RST_POWERON || reason == ESP_RST_INT_WDT)) {        
        ESP_LOGI(TAG,"Non-power-on reset detected. Forcing full reset...");
        //force_hard_reset();
        WRITE_PERI_REG(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_SW_SYS_RST);
        esp_rom_delay_us(2000);  // Give time for reset to take effect        
    }
}