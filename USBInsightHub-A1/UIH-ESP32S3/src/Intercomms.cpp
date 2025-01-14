
#include "Intercomms.h"

static const char* TAG = "Intercoms";

GlobalState *glState;
GlobalConfig *glConfig;

TaskHandle_t inter_pac_alert_handle;
SemaphoreHandle_t i2c_Semaphore;

TwoWire I2CB2B = TwoWire(0);
BaseMCU bMCU;
PAC194x bMeter;

//BaseMCUState prevMCUState[3];
BaseMCUStateOut prevMCUConfig[3];
MeterState prevMeterState[3];
MeterConfig prevMeterConfig[3];

//Route the power meter physical channels to the board channels
uint8_t meterBoardMap[3]={2,0,1}; 
//Route the board channels to power meter physical channels
uint8_t boardMeterMap[3]={1,2,0};

//Internal functions
void taskIntercomms(void *pvParameters);
void interMcuWriteAll(void);
void interMcuReadAll(void);
void interSetCurrentLimits(void);
void interAvgMeterRead(void);

void interPacAlertInterruptHandler(void);
void inter_pac_alert_handler_task(void *pvParameters);
void IRAM_ATTR inter_pac_alert_isr(void);


void iniIntercomms(GlobalState *globalState, GlobalConfig *globalConfig){
    
    glState = globalState;
    glConfig = globalConfig;

    pinMode(PAC_ALERT,INPUT_PULLUP);
    pinMode(MCU_INT,INPUT_PULLUP);

    i2c_Semaphore = xSemaphoreCreateMutex();

    //I2C initialization
    ESP_LOGI(TAG,"Core %u",xPortGetCoreID());
                 
    if(i2c_Semaphore == NULL) ESP_LOGE(TAG, "I2C Semaphore Creation failed");

    if(xSemaphoreTake(i2c_Semaphore,( TickType_t ) 10 ) == pdTRUE)
    {
        I2CB2B.begin(B2B_SDA, B2B_SCL, 400000);
        I2CB2B.setTimeOut(20);
            
        delay(20);
        //if(bMCU.begin(&I2CB2B)) ESP_LOGI(TAG, "Base MCU initialized OK");
        bMCU.begin(&I2CB2B) ? ESP_LOGI(TAG, "Base MCU initialized OK") : ESP_LOGE(TAG, "Base MCU initialization FAILED!");
        delay(30); //required time after powerup to write to meter
        if(bMeter.begin(&I2CB2B)) ESP_LOGI(TAG, "Power Meter initialized OK");
        //clear any interrupt flag
        uint8_t flags = bMeter.readInterruptFlags();
        
        xSemaphoreGive(i2c_Semaphore);

        attachInterrupt(PAC_ALERT, inter_pac_alert_isr, FALLING);        
        xTaskCreatePinnedToCore(taskIntercomms, "Default View", 2560, NULL, 2, NULL,APP_CORE);
        xTaskCreatePinnedToCore(inter_pac_alert_handler_task, "pac alert handler", 2048,  NULL, 2,  &inter_pac_alert_handle, APP_CORE);
    } 
    else {
        ESP_LOGE(TAG, "I2C Hardware is bussy, could not initialize I2C peripherals");        
    }

}


void interMcuWriteAll(void){

  if(i2c_Semaphore != NULL){
    if(xSemaphoreTake(i2c_Semaphore,portMAX_DELAY) == pdTRUE){
      bMCU.writeAll();       
      xSemaphoreGive(i2c_Semaphore);
    } 
    else{
      ESP_LOGE(TAG,"Timeout to write I2C mcu");
    }
  }

}


void interMcuReadAll(void){

  if(i2c_Semaphore != NULL){
    if(xSemaphoreTake(i2c_Semaphore,portMAX_DELAY) == pdTRUE){
      bMCU.readAll();       
      xSemaphoreGive(i2c_Semaphore);
    } 
    else{
      ESP_LOGE(TAG,"Timeout to read I2C mcu");
    }
  }

}

void interAvgMeterRead(void){
  if(i2c_Semaphore != NULL){
    if(xSemaphoreTake(i2c_Semaphore,portMAX_DELAY) == pdTRUE){
      bMeter.refresh_v();
      delayMicroseconds(500); //required to update Meter registers after refresh command. 
      bMeter.readAvgMeter();        
      xSemaphoreGive(i2c_Semaphore);
    } 
    else{
      ESP_LOGE(TAG,"Timeout to get access to I2C read meter");
    }
  }  
}

void interSetCurrentLimits(void){
  
  if(i2c_Semaphore != NULL){
    if(xSemaphoreTake(i2c_Semaphore,portMAX_DELAY) == pdTRUE){
      bMeter.enableAlerts(false);
      ESP_LOGV(TAG,"Set current");
      for(int i=0; i<3; i++)
      {
        bMeter.chMeterArr[i].fwdCLim = glConfig->meter[i].fwdCLim;
        //glState->system.bMeter.chMeterArr[i].backCLim = glConfig->meter[i].backCLim;
        bMeter.setCurrentLimit(glConfig->meter[i].fwdCLim, FORWARD, i);
        ESP_LOGV(TAG,"Fwd Current %i: %s",i,String(glConfig->meter[i].fwdCLim));
        //glState->system.bMeter.setCurrentLimit(glConfig->meter[i].backCLim, BACKWARD, i);
      }
      bMeter.enableAlerts(true);
      xSemaphoreGive(i2c_Semaphore);
    } 
    else{
      ESP_LOGE(TAG,"Timeout to write I2C PAC1943");
    }
  }  
}

void taskIntercomms(void *pvParameters){
  TickType_t xLastWakeTime = xTaskGetTickCount();
  ESP_LOGI(TAG,"Intercomms started on Core %u",xPortGetCoreID());
  for(;;){


    //check if there is any change in GlobalConfig to update the MCU
    if( memcmp(&prevMCUConfig,&(glState->baseMCUOut),sizeof(prevMCUConfig)) != 0 ){
    //if( memcmp(&prevMCUConfig,&(glState->baseMCU),sizeof(prevMCUConfig)) != 0 ){
      //update bMCU data with what is in globalConfig
      ESP_LOGV(TAG, "baseMCU Out changed");
      for(int i=0; i<3; i++){
        bMCU.chArr[i].pwr_en = glState->baseMCUOut[i].pwr_en;
        bMCU.chArr[i].data_en = glState->baseMCUOut[i].data_en;
        bMCU.chArr[i].ilim = glState->baseMCUOut[i].ilim;
      }   
      //send data
      //ESP_LOGV(TAG, "baseMCU mcuwriteall");
      interMcuWriteAll();
      //ESP_LOGV(TAG, "memcpy baseMCU");
      //save current state for later comparison
      if(bMCU.initiated) saveMCUState();          
      memcpy(prevMCUConfig,glState->baseMCUOut,sizeof(prevMCUConfig));
    }

    //read bMCU
    interMcuReadAll();

    //update globalState with bMCU readings
    for(int i=0; i<3; i++){
      glState->baseMCUIn[i].fault = bMCU.chArr[i].fault;
      glState->baseMCUOut[i].pwr_en = bMCU.chArr[i].pwr_en;
      glState->baseMCUOut[i].data_en = bMCU.chArr[i].data_en;
      glState->baseMCUOut[i].ilim = bMCU.chArr[i].ilim;
    }  

    vTaskDelay(pdMS_TO_TICKS(5));
    
    //check if there is any change in GlobalConfig to update the Meter
    if(memcmp(&prevMeterConfig,&(glConfig->meter),sizeof(prevMeterConfig))!=0){
      //update Meter with what is in globalConfig and send to Meter
      ESP_LOGV(TAG, "Meter Config changed");
      interSetCurrentLimits();
      //save current state for later comparison
      ESP_LOGV(TAG, "memcpy Meter");
      memcpy(prevMeterConfig,glConfig->meter,sizeof(prevMeterConfig));         
    }

    //read Meter
    //ESP_LOGV(TAG, "Meter read");
    interAvgMeterRead();
    //Serial.println("%.2f %.2f %.2f",bMeter.chMeterArr[0].AvgVoltage,bMeter.chMeterArr[1].AvgVoltage,bMeter.chMeterArr[2].AvgVoltage);
    //Serial.println(String(bMeter.chMeterArr[0].AvgVoltage) +","+String(bMeter.chMeterArr[1].AvgVoltage)+","+String(bMeter.chMeterArr[2].AvgVoltage));
    
    //update globalState Meter IO
    for(int i=0; i<3; i++){
      //Meter Outputs->State 
      glState->meter[i].AvgCurrent = bMeter.chAverager[meterBoardMap[i]].CurrentAveraged;
      glState->meter[i].AvgVoltage = bMeter.chAverager[meterBoardMap[i]].VoltageAveraged;
      //State->Meter Inputs
      bMeter.chMeterArr[i].backAlertSet = glState->meter[i].backAlertSet;
      bMeter.chMeterArr[i].fwdAlertSet  = glState->meter[i].fwdAlertSet;    
    }
    
    vTaskDelayUntil(&xLastWakeTime,pdMS_TO_TICKS(INTERCOMMS_PERIOD));
  }
}

void interPacAlertInterruptHandler(void){
  //ESP_LOGV(TAG,"PACI");
  BaseType_t xHigherPriorityTaskWoken;
  xHigherPriorityTaskWoken = pdFALSE;
  xTaskNotifyFromISR(inter_pac_alert_handle, 0,eNoAction, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR( xHigherPriorityTaskWoken);
}


void inter_pac_alert_handler_task(void *pvParameters)
{
  uint32_t value;
  
  ESP_LOGV(TAG, "pac_alert_handler_task started");
  for(;;){
    if(xTaskNotifyWait(0x00,0x01,&value, portMAX_DELAY)){
      ESP_LOGI(TAG,"pac alert interrupt detected");
      if(i2c_Semaphore != NULL){
        if(xSemaphoreTake(i2c_Semaphore,( TickType_t ) 10 ) == pdTRUE){        
          //HWSerial.println("Inside sempahore");
          //HWSerial.println(String(bMeter.readInterruptFlags()));
          int ret_count =0; //
          
          //The while loop is added to detect cases in which the alert is triggered again after reading the flags
          //but the function has not finished with the remaining tasks. 
          while(!digitalRead(PAC_ALERT) && ret_count < CLEAR_ALERT_RETRIES)
          {
            uint8_t flags = bMeter.readInterruptFlags();
            
            for(int i=0; i<3; i++)
            {
              //Over Current flags in upper nibble - backward current
              if(flags & 0x80){
                bMCU.chArr[boardMeterMap[i]].pwr_en = false;
                bMeter.chMeterArr[boardMeterMap[i]].backAlertSet = true;
                glState->meter[boardMeterMap[i]].backAlertSet = true;
                ESP_LOGI(TAG,"Back current on CH %s", String(boardMeterMap[i]+1));
              }
              //Under Current flags in lower nibble - forward current
              if(flags & 0x08){
                bMCU.chArr[boardMeterMap[i]].pwr_en = false;
                bMeter.chMeterArr[boardMeterMap[i]].fwdAlertSet = true;
                glState->meter[boardMeterMap[i]].fwdAlertSet = true;
                ESP_LOGI(TAG,"Over current on CH %s", String(boardMeterMap[i]+1));
              }

              flags = flags<<1;
            }    
            //turn off affected channels
            bMCU.writeAll(); //Update the registers to take action in bMCU.
            //update higher level status
            ret_count++;
          }
          ret_count =0;          

          xSemaphoreGive(i2c_Semaphore);
        } 
        else{
          ESP_LOGE(TAG,"Timeout to get access to I2C pac alert");
        }
      }           
    }
  }
}


void IRAM_ATTR inter_pac_alert_isr(void){ 
  interPacAlertInterruptHandler(); 
}