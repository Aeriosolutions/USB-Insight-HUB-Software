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

//Button interrupt handler and resolve functions

#include "Buttons.h"

//Button variables and Interupt functions
Button ButtonArray[4] = { {BUTTON_1,false,true,0,0,0,0,false},
                          {BUTTON_2,false,true,0,0,0,0,false},
                          {BUTTON_3,false,true,0,0,0,0,false},
                          {SETUP,false,true,0,0,0,0,false}
                        };

//Button Setup_Button = {SETUP,false,true,0,0,0,0,false};

ButtonsState buttonsFlags;

static const char* TAG = "Buttons";

void iniButtons(void){

  for (int i=0; i<4; i++) {
    buttonsFlags.shortPress[i]=false;
    buttonsFlags.longPress[i]=false;
  } 

  pinMode(BUTTON_1,INPUT_PULLUP);
  pinMode(BUTTON_2,INPUT_PULLUP);
  pinMode(BUTTON_3,INPUT_PULLUP);
  pinMode(SETUP,INPUT_PULLUP);    

  attachInterrupt(ButtonArray[0].PIN, ch1_btn_isr, FALLING);
  attachInterrupt(ButtonArray[1].PIN, ch2_btn_isr, FALLING);
  attachInterrupt(ButtonArray[2].PIN, ch3_btn_isr, FALLING);
  attachInterrupt(ButtonArray[3].PIN, setup_btn_isr, FALLING);

  xTaskCreate(taskScanLoop, "Button Scan", 1024, NULL, 3, NULL);   
}

void taskScanLoop(void *pvParameters)
{ 
  TickType_t xLastWakeTime = xTaskGetTickCount();
  int tot = 0;
  for(;;){    
    for (int i=0; i<4; i++){
      
      tot=resolveButton(&ButtonArray[i]);
      if(tot==1){
        buttonsFlags.shortPress[i] = true;
      }
      if(tot==2){
        buttonsFlags.longPress[i] = true;
      }
    }
    
    //vTaskDelay(pdMS_TO_TICKS(BUTTON_CHECK_PERIOD));
    vTaskDelayUntil(&xLastWakeTime,pdMS_TO_TICKS(BUTTON_CHECK_PERIOD));
  }
}

bool btnShortCheck(int index){
  if(buttonsFlags.shortPress[index]){
    buttonsFlags.shortPress[index] = false;
    return true;
  }
  
  return false;
}

bool btnLongCheck(int index){
  if(buttonsFlags.longPress[index]){
    buttonsFlags.longPress[index] = false;
    return true;
  }
  
  return false;
}

void btnClearAll(){
  for(int i=0; i<4; i++)
  {
    buttonsFlags.shortPress[i] = false;
    buttonsFlags.longPress[i] = false;
  }
}

//Handler for button interruption
void btnDebounce(Button *button)
{
  button->button_time = millis();
  if ((button->button_time - button->last_button_time > DEBOUNCE_TIME) && button->high_again){          
        button->pressed = true;
        button->last_button_time = button->button_time;        
  }   
}

//check if a button was pressed and the debounce time is finished
int resolveButton(Button *button)
{
  int ret=0;
  
  if(button->counter > BACK_TO_HIGH){
    button->counter      = 0;
    button->high_again   = true;
    button->long_pressed = false;
  }

  if(button->pressed && button->high_again)
  {
    button->pressed    = false;
    button->high_again = false;
  }

  if(button->high_again)
  {

    if(button->low_counter >= BUTTON_SHORT_LOW && button->low_counter < BUTTON_SHORT_HIGH) {   
      ret=1;
    }    
    button->low_counter = 0;
  }

  if(button->low_counter >= BUTTON_LONG && !button->long_pressed)
  {
    button->long_pressed = true;
    ret = 2;
  }

  if(!digitalRead(button->PIN) && !button->high_again){
    button->counter = 0;
    button->low_counter++;
  } 
  if(digitalRead(button->PIN) && !button->high_again){
    button->counter++;
  }  

	return ret;
}

void IRAM_ATTR ch1_btn_isr() {
    btnDebounce(&ButtonArray[0]);
}

void IRAM_ATTR ch2_btn_isr() {
    btnDebounce(&ButtonArray[1]);
}

void IRAM_ATTR ch3_btn_isr() {
    btnDebounce(&ButtonArray[2]);
}

void IRAM_ATTR setup_btn_isr() {
    btnDebounce(&ButtonArray[3]);
}