#ifndef DEFAULTVIEW_H
#define DEFAULTVIEW_H

#include <Arduino.h>
#include "Buttons.h"
#include "datatypes.h"
#include "Screen.h"
#include "MenuView.h"

#define DEFAULT_VIEW_PERIOD      40 //in ms
#define SLOW_DATA_DOWNSAMPLES_0_5    10 //500ms //multiples of DISPLAY_REFRESH_PERIOD 
#define SLOW_DATA_DOWNSAMPLES_1_0    20 //1000ms //multiples of DISPLAY_REFRESH_PERIOD 


void iniDefaultView(GlobalState* globalState, GlobalConfig* globalConfig, Screen *screen);
void defaultViewStart(void);

extern SemaphoreHandle_t screen_Semaphore;

#endif