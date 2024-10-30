//Add licence text
//The Overcurrent and Undercurrent events must be configured in the PAC1943 to generate an interrupt. 
//When the interrupt in pin PAC_ALERT is catched, pacAlertInterruptHandler is called and notifies pac_alert_handler_task.
//pacAlertInterruptHandler must be kept as short as possible.  

#define CLEAR_ALERT_RETRIES 3

//void CurrentEventsManager(PAC194x *t_meter,BaseMCU *t_bmcu,TwoWire *t_i2cb2b)

void pacAlertInterruptHandler(void){
  BaseType_t xHigherPriorityTaskWoken;
  xHigherPriorityTaskWoken = pdFALSE;
  xTaskNotifyFromISR(pac_alert_handle, 0,eNoAction, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR( xHigherPriorityTaskWoken);
}

void pac_alert_handler_task(void *pvParameters)
{
  uint32_t value;
  //int Map[3]={1,2,0};

  for(;;){
    if(xTaskNotifyWait(0x00,0x01,&value, portMAX_DELAY)){
      
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
                bMCU.chArr[invmeterMap[i]].pwr_en = false;
                bMeter.chMeterArr[invmeterMap[i]].backAlertSet = true;
                HWSerial.println("Back current on CH" + String(invmeterMap[i]+1));
              }
              //Under Current flags in lower nibble - forward current
              if(flags & 0x08){
                bMCU.chArr[invmeterMap[i]].pwr_en = false;
                bMeter.chMeterArr[invmeterMap[i]].fwdAlertSet = true;
                HWSerial.println("Over current on CH" + String(invmeterMap[i]+1));
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
          HWSerial.println("Timeout to get access to I2C pac alert");
        }
      }           
    }
  }
}
