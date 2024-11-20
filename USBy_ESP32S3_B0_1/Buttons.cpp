//Add licence text
//Button interrupt handler and resolve functions

#include <Arduino.h>
#include "Buttons.h"

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