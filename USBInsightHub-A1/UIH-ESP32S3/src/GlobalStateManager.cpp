#include "GlobalStateManager.h"

static const char* TAG = "GlobalStateManager";

Preferences flashstorage;

GlobalState *globlState;
GlobalConfig *globlConfig;

void setDefaultGlobalConfig(GlobalState *globalState, GlobalConfig *globalConfig);


void globalStateInitializer(GlobalState *globalState, GlobalConfig *globalConfig){

    globlState  = globalState;
    globlConfig = globalConfig;

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

    //---Features
    globalConfig->features.startUpmode != STARTUP_SEC ? globalState->features.startUpActive = false : globalState->features.startUpActive = true;
    globalState->features.pcConnected   = false;
    globalState->features.vbusVoltage   = 5.000; //Fixed value, Not implemented yet
    globalState->features.wifiState     = WIFI_UNKNOWN;
    globalState->features.wifiAPIP      = "192.168.1.1";
    globalState->features.wifiIP        = "0.0.0.0";

    
    for(int i=0; i<3; i++){
        //---Startup    
        globalState->startup[i].startup_cnt = globalConfig->startup[i].startup_timer;
        
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
                globalState->startup[i].startup_cnt = 0;
                break;
            case START_ON:
                globalState->startup[i].startup_cnt = 0;
                globalState->baseMCUOut[i].data_en = true;
                globalState->baseMCUOut[i].pwr_en = true;
                break;
            case START_OFF: 
            case STARTUP_SEC:                
                globalState->baseMCUOut[i].data_en = true;
                globalState->baseMCUOut[i].pwr_en = false;
                break;
        }

    }

    //---BaseMCUExtra
    globalState->baseMCUExtra.vext_cc = UNKNOWN;
    globalState->baseMCUExtra.vhost_cc = UNKNOWN;
    globalState->baseMCUExtra.vext_stat = NO_PULLUPS;
    globalState->baseMCUExtra.vhost_stat = NO_PULLUPS;
    globalState->baseMCUExtra.pwr_source = VHOST;  
    globalState->baseMCUExtra.usb3_mux_out_en = false;
    globalState->baseMCUExtra.usb3_mux_sel_pos = false;
    globalState->baseMCUExtra.base_ver = 255; //not implemented yet

}

void setDefaultGlobalConfig(GlobalState *globalState, GlobalConfig *globalConfig){
    
    globalConfig->features.startView = DEFAULT_VIEW;
    globalConfig->features.startUpmode = PERSISTANCE;
    //globalConfig->features.startUpmode = STARTUP_SEC;
    globalConfig->features.wifi_enabled = true;
    globalConfig->features.hubMode = USB3;

    for(int i=0; i<3; i++){
        globalConfig->startup[i].startup_timer = 1;

        globalConfig->screen[i].rotation = ROT_180_DEG;
        globalConfig->screen[i].brightness = 800;  

        globalConfig->meter[i].backCLim = 20; //mA
        globalConfig->meter[i].fwdCLim = 1000; //mA
        globalConfig->meter[i].filterType = FILTER_TYPE_MEDIAN;

        globalState->baseMCUOut[i].ilim = ILIM_1_0;
        globalState->baseMCUOut[i].data_en = true;
        globalState->baseMCUOut[i].pwr_en = true;
        
    }
    //for test only
    globalConfig->startup[0].startup_timer = 41;
    globalConfig->startup[1].startup_timer = 61;
    globalConfig->startup[2].startup_timer = 81;
}

void saveMCUState(void){
    flashstorage.begin(UIH_NAMESPACE,false);
    flashstorage.putBytes("MCUBlob",&(globlState->baseMCUOut),sizeof(globlState->baseMCUOut));
    ESP_LOGI(TAG,"Save MCU blob");
    flashstorage.end();
}