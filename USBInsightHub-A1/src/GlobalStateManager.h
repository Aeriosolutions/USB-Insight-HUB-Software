#ifndef GLOBALSTATEMANAGER_H
#define GLOBALSTATEMANAGER_H

#include <Arduino.h>
#include "datatypes.h"
#include <Wire.h>
#include "BaseMCU.h"
#include "PAC194x.h"
#include <Preferences.h>

#define UIH_NAMESPACE "uih-nvm-1"
#define MEM_INITIALIZED_NUM 55

void globalStateInitializer(GlobalState *globalState, GlobalConfig *globalConfig);
void saveMCUState(void);

#endif