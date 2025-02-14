#ifndef DEFAULTVIEW_H
#define DEFAULTVIEW_H

#include <Arduino.h>
#include "Buttons.h"
#include "datatypes.h"
#include "Screen.h"
#include "MenuView.h"

#define DEFAULT_VIEW_PERIOD      40 //in ms



void iniDefaultView(GlobalState* globalState, GlobalConfig* globalConfig, Screen *screen);
void defaultViewStart(void);

extern SemaphoreHandle_t screen_Semaphore;

#endif