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
        xTaskCreate(taksPowerStartUp, "Power Startup", 2048, NULL, 4, NULL);

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