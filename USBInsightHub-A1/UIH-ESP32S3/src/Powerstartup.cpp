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

// task for the power startup logic, when enabled

#include "Powerstartup.h"

GlobalState *globState;
GlobalConfig *globConfig;

void taksPowerStartUp(void *pvParameters);


void iniPowerStartUp(GlobalState* globalState, GlobalConfig* globalConfig){

    globState = globalState;
    globConfig = globalConfig;

    if(globConfig->features.startUpmode == STARTUP_SEC){
        globState->features.startUpActive = true;
    }             

    if(globState->features.startUpActive) //start if feature enabled
    {
        xTaskCreate(taksPowerStartUp, "Power Startup", 2048, NULL, 4, NULL);
        ESP_LOGI("startup","Startup Start %u",millis());
    }

}

void taksPowerStartUp(void *pvParameters){
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for(;;){
        if(globState->features.startUpActive)
        {
            for(int i=0; i<3; i++)
            {        
                if(globState->startup[i].startup_cnt==1){
                    globState->baseMCUOut[i].pwr_en = true;                   
                    ESP_LOGI("startup","Timeout CH %s",String(i+1));
                }        
                if(globState->startup[i].startup_cnt>0) 
                    globState->startup[i].startup_cnt--;                
            }

            if(globState->startup[0].startup_cnt<=0 && globState->startup[1].startup_cnt<=0 && globState->startup[2].startup_cnt<=0)  
                globState->features.startUpActive = false;

            //vTaskDelay(pdMS_TO_TICKS(STARTUP_TIMER_PERIOD));
            vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(STARTUP_TIMER_PERIOD));
        }
        else{
        //HWSerial.println("End start up task");
        ESP_LOGI("startup","End start up task");
        vTaskDelete(NULL);
        }
    }
}