//Add licence text

#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>
#include "datatypes.h"

//pin definitions in datatypes.h

#define BUTTON_CHECK_PERIOD            40 //in ms

//Buttons timing calibration
#define DEBOUNCE_TIME     250
#define BACK_TO_HIGH        2 //number of BUTTON_CHECK_PERIODs
#define BUTTON_SHORT_LOW    1
#define BUTTON_SHORT_HIGH  10
#define BUTTON_LONG        20

struct Button {
  const uint8_t PIN;
  bool pressed;
  bool high_again;
  unsigned long button_time;
  unsigned long last_button_time; //variables to keep track of the timing of recent interrupts
  unsigned long counter;
  unsigned long low_counter;
  bool long_pressed;
};

struct ButtonsState {
  bool shortPress[4];
  bool longPress[4];
};

void btnDebounce(Button *button);

void IRAM_ATTR ch1_btn_isr();
void IRAM_ATTR ch2_btn_isr(); 
void IRAM_ATTR ch3_btn_isr(); 
void IRAM_ATTR setup_btn_isr(); 

extern void iniButtons();
void taskScanLoop(void *pvParameters);

extern int resolveButton(Button *button);

bool btnShortCheck(int index);
bool btnLongCheck(int index);

#endif