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



#include <Arduino.h>

#include <ESP32SvelteKit.h>
#include <PsychicHttpServer.h>
#include <MasterStateService.h>

#include "datatypes.h"
#include "GlobalStateManager.h"
#include "Intercomms.h"
#include "Extercomms.h"
#include "DefaultView.h"
#include "Powerstartup.h"

#include <ArduinoJson.h>

#define SERIAL_BAUD_RATE 115200

GlobalState globalState ={};
GlobalConfig globalConfig ={};
Screen screen;

PsychicHttpServer server;

ESP32SvelteKit esp32sveltekit(&server, 160); //defaul 120

MasterStateService masterStateService = MasterStateService(&server,
                                                        esp32sveltekit.getSocket(),
                                                        esp32sveltekit.getSecurityManager());                                                        


void setup()
{

    // start serial and filesystem
    //Serial.begin(SERIAL_BAUD_RATE);   
    globalStateInitializer(&globalState,&globalConfig);
    iniIntercomms(&globalState, &globalConfig);
    delay(10);
    iniPowerStartUp(&globalState,&globalConfig);     
    iniExtercomms(&globalState,&globalConfig);
    delay(40); //to give time to print
    ESP_LOGI("Main","Running Firmware Version: %s\n", APP_VERSION);

    iniButtons();
    iniDefaultView(&globalState,&globalConfig, &screen);
       
    
    // start ESP32-SvelteKit if WiFi is enabled
    if(globalConfig.features.wifi_enabled == ENABLE){
        esp32sveltekit.begin();    
        masterStateService.begin(&globalState,&globalConfig,&esp32sveltekit);
    }
    
}

void loop()
{   

    //ESP_LOGI("Main"," Heap_ Free: %u, Total: %u, MinFree: %u, MaxAlloc: %u", ESP.getFreeHeap(), ESP.getHeapSize(),ESP.getMinFreeHeap(),ESP.getMaxAllocHeap());

    delay(1000);    
}
