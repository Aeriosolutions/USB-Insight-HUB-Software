#ifndef EXTERCOMMS_H
#define EXTERCOMMS_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include "USB.h"
#include "datatypes.h"
#include <ArduinoJson.h>

#define PC_CONNECTION_TIMEOUT   2500
#define SERIAL_CHECK_PERIOD     50         
#define MAX_BUFFER_SIZE         512

#define ARR_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

void iniExtercomms(GlobalState* globalState,GlobalConfig* globalConfig);


#endif