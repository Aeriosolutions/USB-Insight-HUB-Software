#ifndef POWERSTARTUP_H
#define POWERSTARTUP_H

#include <Arduino.h>
#include "datatypes.h"

#define STARTUP_TIMER_PERIOD 100 //ms

void iniPowerStartUp(GlobalState* globalState, GlobalConfig* globalConfig);

#endif //POWERSTARTUP_H
