//Add licence text

/*Definitions of functions and variables of processes related to the I2C
communitcaion between the Main MCU, the BaseMCU and the Power Meter
*/

#ifndef INTERCOMMS_H
#define INTERCOMMS_H

#include <Arduino.h>
#include <Wire.h>
#include "BaseMCU.h"
#include "PAC194x.h"
#include "datatypes.h"
#include "GlobalStateManager.h"

//pin definitions in datatypes.h

#define CLEAR_ALERT_RETRIES 3

#define INTERCOMMS_PERIOD 40 //ms

void iniIntercomms(GlobalState *globalState, GlobalConfig *globalConfig);


#endif