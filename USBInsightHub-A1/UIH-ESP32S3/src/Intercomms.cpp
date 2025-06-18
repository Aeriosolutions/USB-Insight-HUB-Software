
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

//Logic related to the I2C communitcaion between the ESP32, the BaseMCU and the Power Meter
//Handles power meter interrupts and analog readings


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
//MeterState prevMeterState[3];
MeterConfig prevMeterConfig[3];
uint8_t prevHubMode;

//Route the power meter physical channels to the board channels
uint8_t meterBoardMap[3]={2,0,1}; 
//Route the board channels to power meter physical channels
uint8_t boardMeterMap[3]={1,2,0};

//ADC
//calibration
esp_adc_cal_characteristics_t adc_chars;
uint32_t adc_samples[ADC_NUMSAMPLES];
int adc_idx = 0;
uint32_t adc_sum = 0;

//Counts the number of I2C read fails of PAC1943 
uint8_t i2cErrCnt = 0;

//Internal functions
void taskIntercomms(void *pvParameters);
void interMcuWriteAll(void);
void interMcuReadAll(void);
void interSetCurrentLimits(void);
void interAvgMeterRead(void);
float read5Vrail(void);
void forcePacTimeout();


void interPacAlertInterruptHandler(void);
void inter_pac_alert_handler_task(void *pvParameters);
void IRAM_ATTR inter_pac_alert_isr(void);


void iniIntercomms(GlobalState *globalState, GlobalConfig *globalConfig){
    
    glState = globalState;
    glConfig = globalConfig;

    pinMode(PAC_ALERT,INPUT_PULLUP);
    pinMode(MCU_INT,INPUT_PULLUP);

    i2c_Semaphore = xSemaphoreCreateMutex();

    //memcpy(prevMCUConfig,glState->baseMCUOut,sizeof(prevMCUConfig));
    //memcpy(prevMeterConfig,glConfig->meter,sizeof(prevMeterConfig));
    //prevHubMode = glConfig->features.hubMode;

    // Characterize ADC (helps improve accuracy)
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
    analogSetPinAttenuation(VBUS_MONITOR, ADC_11db);

    //I2C initialization
    ESP_LOGI(TAG,"Core %u",xPortGetCoreID());
                 
    if(i2c_Semaphore == NULL) ESP_LOGE(TAG, "I2C Semaphore Creation failed");

    if(xSemaphoreTake(i2c_Semaphore,( TickType_t ) 10 ) == pdTRUE)
    {
        I2CB2B.begin(B2B_SDA, B2B_SCL, I2CSPEED);
        I2CB2B.setTimeOut(20);
        
            
        delay(20);
        //if(bMCU.begin(&I2CB2B)) ESP_LOGI(TAG, "Base MCU initialized OK");
        if(bMCU.begin(&I2CB2B))
        {
          bMCU.readVersion();
          ESP_LOGI(TAG, "Base MCU initialized OK version %u",bMCU.baseMCUVer);
          if(bMCU.baseMCUVer != COMPATIBLE_BMCU_VER) {            
            glState->system.internalErrFlags |= BMCU_VER_ERR;
          }
          if(glConfig->features.hubMode==USB2_3 ||glConfig->features.hubMode==USB3)
            bMCU.setUSB3Enable(true);
          if(glConfig->features.hubMode==USB2)
            bMCU.setUSB3Enable(false);
        }
        else {
          ESP_LOGE(TAG, "Base MCU initialization FAILED!");
          glState->system.internalErrFlags |= BMCU_INIT_ERR;
        }
          
        
        delay(30); //required time after powerup to write to meter
        if(bMeter.begin(&I2CB2B)) {
          ESP_LOGI(TAG, "Power Meter initialized OK. Interrupt pin: %s",bMeter.getIntTestResult() ? "OK": "FAIL");
          if(!bMeter.getIntTestResult()){            
            glState->system.internalErrFlags |= PAC_INT_PIN_ERR;
          }
          glState->system.meterInit = METER_INIT_OK;
          glState->system.pacRevisionID = bMeter.getRevisionID();
        }
        else
        {
          ESP_LOGE(TAG, "Power Meter initialization FAILED!");
          glState->system.meterInit = METER_INIT_FAILED;
          glState->system.internalErrFlags |= PAC_INIT_ERR;
        } 
        //clear any interrupt flag
        uint8_t flags = bMeter.readInterruptFlags();
        
        xSemaphoreGive(i2c_Semaphore);

        attachInterrupt(PAC_ALERT, inter_pac_alert_isr, FALLING);        
        xTaskCreatePinnedToCore(taskIntercomms, "Intercomms", 7168, NULL, 1,&(glState->system.taskIntercommHandle),APP_CORE);
        xTaskCreatePinnedToCore(inter_pac_alert_handler_task, "pac alert handler", 2048,  NULL, 1,  &inter_pac_alert_handle, APP_CORE);
    } 
    else {
        ESP_LOGE(TAG, "I2C Hardware is bussy, could not initialize I2C peripherals");        
    }
    ESP_LOGI(TAG, "EF: %s", String(glState->system.internalErrFlags,HEX));
}


float read5Vrail(void){
  
  uint32_t lin_value = esp_adc_cal_raw_to_voltage(analogRead(VBUS_MONITOR), &adc_chars);
  adc_sum -= adc_samples[adc_idx];  // Remove oldest value from sum
  adc_samples[adc_idx] = lin_value;  // Store new value
  adc_sum += lin_value;  // Add new value to sum

  adc_idx = (adc_idx + 1) % ADC_NUMSAMPLES;  // Move index in circular buffer

  float average = (adc_sum / (float)ADC_NUMSAMPLES)*DIV5VRATIO;  
  
  //ESP_LOGI(TAG, "voltage: %f",average);
  return average;
}

void interMcuWriteAll(void){

  if(i2c_Semaphore != NULL){
    if(xSemaphoreTake(i2c_Semaphore,pdMS_TO_TICKS(10)) == pdTRUE){
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
    if(xSemaphoreTake(i2c_Semaphore,pdMS_TO_TICKS(10)) == pdTRUE){
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
    if(xSemaphoreTake(i2c_Semaphore,pdMS_TO_TICKS(10)) == pdTRUE){

      //resets I2C bus if read errors persist
      if(i2cErrCnt > 10){
        ESP_LOGE(TAG,"Try reset I2C bus after %i PAC1943 read failures",i2cErrCnt);
        forcePacTimeout();
        i2cErrCnt = 0;
      }
            
      delayMicroseconds(1000);
      bMeter.readAvgMeter();
      bMeter.refresh(0);        
      xSemaphoreGive(i2c_Semaphore);

      if (bMeter.getError()==0){
        glState->system.meterInit = METER_INIT_OK;
        i2cErrCnt = 0;
      } 
      else if (bMeter.getError()==1){
        glState->system.meterInit = METER_INIT_READ_ERR;
        i2cErrCnt++;        
      } 
      else if (bMeter.getError()==2) glState->system.meterInit = METER_INIT_SLOW_ERR;
    } 
    else{
      ESP_LOGE(TAG,"Timeout to get access to I2C read meter");
    }
  }  
}

void forcePacTimeout(){
  I2CB2B.end();
  pinMode(B2B_SCL, OUTPUT_OPEN_DRAIN);
  pinMode(B2B_SDA, OUTPUT_OPEN_DRAIN);
  digitalWrite(B2B_SDA, HIGH);
  //delayMicroseconds(5);
  digitalWrite(B2B_SCL, LOW);
  vTaskDelay(pdMS_TO_TICKS(26));
  
  I2CB2B.begin(B2B_SDA, B2B_SCL, 400000);
  I2CB2B.setTimeOut(20);  
}


void interSetCurrentLimits(void){
  
  if(i2c_Semaphore != NULL){
    if(xSemaphoreTake(i2c_Semaphore,pdMS_TO_TICKS(10)) == pdTRUE){
      bMeter.enableAlerts(false);
      ESP_LOGV(TAG,"Set current");
      for(int i=0; i<3; i++)
      {
        if(prevMeterConfig[i].fwdCLim !=glConfig->meter[i].fwdCLim){
          bMeter.chMeterArr[meterBoardMap[i]].fwdCLim = glConfig->meter[i].fwdCLim;          
          bMeter.setCurrentLimit(glConfig->meter[i].fwdCLim, FORWARD, meterBoardMap[i]);
          ESP_LOGV(TAG,"Fwd Current %i: %s",i,String(glConfig->meter[i].fwdCLim));
        }        
        if(prevMeterConfig[i].backCLim != glConfig->meter[i].backCLim){
          bMeter.chMeterArr[meterBoardMap[i]].backCLim = glConfig->meter[i].backCLim;          
          bMeter.setCurrentLimit(glConfig->meter[i].backCLim, BACKWARD, meterBoardMap[i]);
          ESP_LOGV(TAG,"Back Current %i: %s",i,String(glConfig->meter[i].backCLim));
        }                         
      }
      bMeter.enableAlerts(true);
      xSemaphoreGive(i2c_Semaphore);
    } 
    else{
      ESP_LOGE(TAG,"Timeout I2C to set current on PAC1943");
    }
  }  
}

void taskIntercomms(void *pvParameters){
  TickType_t xLastWakeTime = xTaskGetTickCount();
  ESP_LOGI(TAG,"Intercomms started on Core %u",xPortGetCoreID());
  unsigned long timer=0;
  bool forceMCUwrite = false; 

  for(;;){

    //wait taskDefaultScreenLoop notification to synchronize DISPLAY_REFRESH_PERIOD
    //with actual sampling rate.
    ulTaskNotifyTake(pdTRUE,pdMS_TO_TICKS(100));

    timer = millis();
    //handle automatic selection of the hardware current limit based on forward current limit
    for(int i=0; i<3; i++){

      if(glConfig->meter[i].fwdCLim <= 500) 
        glState->baseMCUOut[i].ilim = 0;
      if(glConfig->meter[i].fwdCLim > 500 && glConfig->meter[i].fwdCLim <= 1000) 
        glState->baseMCUOut[i].ilim = 1;
      if(glConfig->meter[i].fwdCLim > 1000 && glConfig->meter[i].fwdCLim <= 1500) 
        glState->baseMCUOut[i].ilim = 2;
      if(glConfig->meter[i].fwdCLim > 1500) 
        glState->baseMCUOut[i].ilim = 3;

    }

    //if there is a change in the HUB mode operation
    if(prevHubMode != glConfig->features.hubMode){
      ESP_LOGI(TAG, "HUB Mode Changed");
      if(glConfig->features.hubMode == USB2_3 ||glConfig->features.hubMode == USB3)
        bMCU.setUSB3Enable(true);
      if(glConfig->features.hubMode == USB2)
        bMCU.setUSB3Enable(false);

      for(int i =0; i< 3; i++) 
      {
        if(glConfig->features.hubMode == USB2_3 || glConfig->features.hubMode == USB2)
          glState->baseMCUOut[i].data_en = true;
        if(glConfig->features.hubMode == USB3)
          glState->baseMCUOut[i].data_en = false;
      }
      prevHubMode = glConfig->features.hubMode;                      
    }

    //check if there is any change in GlobalConfig to update the MCU
    if( memcmp(&prevMCUConfig,&(glState->baseMCUOut),sizeof(prevMCUConfig)) != 0 || forceMCUwrite){
      forceMCUwrite = false;
      //update bMCU data with what is in globalConfig
      ESP_LOGI(TAG, "baseMCU Out changed");
      for(int i=0; i<3; i++){
        bMCU.chArr[i].pwr_en = glState->baseMCUOut[i].pwr_en;
        bMCU.chArr[i].data_en = glState->baseMCUOut[i].data_en;
        bMCU.chArr[i].ilim = glState->baseMCUOut[i].ilim;
      }   
      //send data
      //ESP_LOGV(TAG, "baseMCU mcuwriteall");
      interMcuWriteAll();

      //Flag to save state only if statupmode is persistance      
      if(bMCU.initiated && glConfig->features.startUpmode == PERSISTANCE) 
        glState->system.saveMCUState = true;        
      //save current state for later comparison
      memcpy(prevMCUConfig,glState->baseMCUOut,sizeof(prevMCUConfig));
    }
    
    //read bMCU
    interMcuReadAll();  

    //update globalState with bMCU readings only if is not first boot
    if(!bMCU.firstboot){
      for(int i=0; i<3; i++){
        glState->baseMCUIn[i].fault = bMCU.chArr[i].fault;
        glState->baseMCUOut[i].pwr_en = bMCU.chArr[i].pwr_en;
        glState->baseMCUOut[i].data_en = bMCU.chArr[i].data_en;
        glState->baseMCUOut[i].ilim = bMCU.chArr[i].ilim;
      } 
    }
    else
    {
      forceMCUwrite = true; //force MCU registers update on next cycle 
      ESP_LOGI(TAG, "bMCU reset detected");
    }


    glState->baseMCUExtra.base_ver = bMCU.baseMCUVer;
    glState->baseMCUExtra.pwr_source = bMCU.pwrsource;
    glState->baseMCUExtra.usb3_mux_out_en = bMCU.muxoe;
    glState->baseMCUExtra.usb3_mux_sel_pos = bMCU.muxsel;
    glState->baseMCUExtra.vext_cc = bMCU.vextCC;
    glState->baseMCUExtra.vhost_cc = bMCU.vhostCC;
    glState->baseMCUExtra.vext_stat = bMCU.extState;
    glState->baseMCUExtra.vhost_stat = bMCU.hostState; 

    //vTaskDelay(pdMS_TO_TICKS(5));
    
    //check if there is any change in GlobalConfig to update the Meter current limits
    if(memcmp(&prevMeterConfig,&(glConfig->meter),sizeof(prevMeterConfig))!=0){
      //update Meter with what is in globalConfig and send to Meter
      ESP_LOGV(TAG, "Meter Config changed");
      interSetCurrentLimits();
      //save current state for later comparison
      ESP_LOGV(TAG, "memcpy Meter");
      memcpy(prevMeterConfig,glConfig->meter,sizeof(prevMeterConfig));         
    }

    for(int i=0; i<3; i++){
      if(glConfig->features.filterType==FILTER_TYPE_MOVING_AVG) bMeter.chMeterArr[i].filterType = FILTER_TYPE_MOVING_AVG;
      if(glConfig->features.filterType==FILTER_TYPE_MEDIAN) bMeter.chMeterArr[i].filterType = FILTER_TYPE_MEDIAN;
    }
    if(glConfig->features.refreshRate==S0_5) bMeter.setFilterLength(SLOW_DATA_DOWNSAMPLES_0_5);
    if(glConfig->features.refreshRate==S1_0) bMeter.setFilterLength(SLOW_DATA_DOWNSAMPLES_1_0);
    //read Meter
    
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
    //ESP_LOGI("I2C","%u",millis()-timer); //----------------------------------
    //this task takes 2 ms

    //Front panel LED update
    digitalWrite(AUX_LED,glState->system.ledState);

    //Read 5V rail voltage
    glState->features.vbus = read5Vrail();

    if(timer > VBUS_STABILIZAION_TIME && glState->features.vbus < VBUS_FAIL_THRES )
    {
      glState->system.internalErrFlags |= VBUS_MONITOR_ERR;
      ESP_LOGI(TAG, "VBUS is below %f",VBUS_FAIL_THRES);
    }
    else {
      glState->system.internalErrFlags &= ~VBUS_MONITOR_ERR;
    }
    
    if(glState->system.taskDefaultScreenLoopHandle != NULL)
      xTaskNotifyGive(glState->system.taskDefaultScreenLoopHandle);
    //vTaskDelayUntil(&xLastWakeTime,pdMS_TO_TICKS(INTERCOMMS_PERIOD));
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