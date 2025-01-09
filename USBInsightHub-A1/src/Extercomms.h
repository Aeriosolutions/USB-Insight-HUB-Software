#ifndef EXTERCOMMS_H
#define EXTERCOMMS_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include "USB.h"
#include "datatypes.h"

#define PC_CONNECTION_TIMEOUT   2500
#define SERIAL_CHECK_PERIOD     50         


void iniExtercomms(GlobalState* globalState);


#endif