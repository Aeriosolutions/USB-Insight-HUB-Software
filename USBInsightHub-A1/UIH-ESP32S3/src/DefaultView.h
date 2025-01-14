#ifndef DEFAULTVIEW_H
#define DEFAULTVIEW_H

#include <Arduino.h>
#include "Buttons.h"
#include "datatypes.h"
#include "Screen.h"

#define DEFAULT_VIEW_PERIOD      40 //in ms
#define SLOW_DATA_DOWNSAMPLES    10 //500ms //multiples of DISPLAY_REFRESH_PERIOD 


void iniDefaultView(GlobalState* globalState, GlobalConfig* globalConfig, Screen *screen);

#endif