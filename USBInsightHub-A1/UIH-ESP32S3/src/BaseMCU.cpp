/**
 *   USB Insight Hub
 *
 *   A USB supercharged interfacing tool for developers & tech enthusiasts wrapped 
 *   around ESP32 SvelteKit framework.
 *   https://github.com/Aeriosolutions/USB-Insight-HUB-Software
 *
 *   Copyright (C) 2024 - 2025 Aeriosolutions
 *   Copyright (C) 2024 - 2025 JoDaSa

 * MIT License. Check full description on LICENSE file.
 **/

 
//Class that implements the drivers for I2C communication with the STM8 coprocessor (Base M)

#include <Arduino.h>
#include "BaseMCU.h"

static const char* TAG = "BaseMCU";

bool BaseMCU::begin(TwoWire *theWire){
  int err  = 0;
  int dat  = 0;
  bool ret = false;

   
  theWire->beginTransmission(BASEMCU_ADDR);
  theWire->write(WHOAMI); //Who am I
  err = theWire->endTransmission(false);  
  err = theWire->requestFrom(BASEMCU_ADDR,1);

  if(theWire->available())
  {
    if(theWire->read()==WHOAMI_ID) 
	{
      I2C = theWire; //if the first connection succeed, save the I2C handler address in the class
      initiated=true;
      ret = true;
    }
  }
  return ret;
}

bool BaseMCU::readStart(int address, int start, int numBytes){
  
  if(initiated){
    int err=0;
    unsigned long i2cwd_timer = 0;

    I2C->beginTransmission(address);
    I2C->write(start);
    err = I2C->endTransmission(false); 
    i2cwd_timer = millis();  //*probably this protection is not longer necessary
    err = I2C->requestFrom(address,numBytes); //*
    i2cwd_timer = millis()-i2cwd_timer;
    if(i2cwd_timer > SLOWDOWN_TIMEOUT_MCU){ //*workaround for sudden drop in I2C speed, reported here https://github.com/espressif/arduino-esp32/issues/8480
      //Serial.println("I2C Slow down on bMCU detected!!");//*
      ESP_LOGW(TAG,"I2C Slow on bMCU: %u!!",i2cwd_timer);
      I2C->flush(); //*
      I2C->setClock(100000); //*
      I2C->setClock(400000); //*
      return false; //*
    } 

    if(err==0) {
      //Serial.println("BaseMCU Fail to read bytes");
      ESP_LOGW(TAG,"BaseMCU Fail to read bytes");
      return false;
    }

    return true;

  }  
  return false;
}

void BaseMCU::readAll(){
  
  uint8_t data;
  int byteCnt = 0;
  int err=0;
  unsigned long i2cwd_timer = 0;

  if(readStart(BASEMCU_ADDR,CH1REG,5)){  

    while(I2C->available()){
      data=I2C->read();

      if(byteCnt < 3) {
        chArr[byteCnt] = parseCHREG(data);
        //Serial.println(String(chArr[byteCnt].fault)+" "+ String(chArr[byteCnt].data_en)+" "+ String(chArr[byteCnt].pwr_en)+" "+ String(chArr[byteCnt].ilim));
      }

      if(byteCnt==3) {
        vextCC = (data & 0x03);
        extState = (data & 0x0C)>>2;
        vhostCC = (data & 0x30)>>4;
        hostState = (data & 0xC0)>>6;
        //ESP_LOGI(TAG, "ECC: %u,ES: %u, HCC: %u, HS: %u", vextCC,extState,vhostCC, hostState);        
      }

      if(byteCnt==4){
        pwrsource = (data & 0x02) == 0 ? false : true;
        muxoe     = (data & 0x04) == 0 ? false : true;
        muxsel    = (data & 0x08) == 0 ? false : true;
        firstboot = (data & 0x10) == 0 ? false : true;
        //ESP_LOGI(TAG, "PS: %u, MOE: %u, MSEL: %u, FB: %u",pwrsource,muxoe,muxsel,firstboot);        
      }
      byteCnt++;
     }
  }
}

Channel BaseMCU::parseCHREG(uint8_t data){

  Channel ch = {false, true, true, 0};

  ch.fault   = ((data & 0x80)==0) ? false : true; 
  ch.data_en = ((data & 0x40)==0) ? false : true;
  ch.pwr_en  = ((data & 0x10)==0) ? false : true;
  ch.ilim = uint8_t((data & 0x06)>>1);

  return ch;
}

void BaseMCU::writeAll(){

  int err=0;
  uint8_t data;
  if(initiated){  
    I2C->beginTransmission(BASEMCU_ADDR);
    I2C->write(CH1REG);
    
    for(int i=0; i<3; i++){
      data=encodeCHREG(chArr[i]);
      I2C->write(data);
      //I2C->write(0xD6);
    }

    err = I2C->endTransmission();
  } 
  else {
    //if not initiated, all output values must be reset to defaults
    for(int i=0; i<3; i++){
      chArr[i].pwr_en = false;
      chArr[i].data_en = false;
      chArr[i].ilim = ILIM_0_5;
    }
  }
}

uint8_t BaseMCU::encodeCHREG(Channel ch){
  uint8_t data = 0;
  data = ch.ilim << 1;
  data = ch.data_en ? (data | 0x40) : (data & 0x3F);
  data = ch.pwr_en  ? (data | 0x10) : (data & 0x6F);
  //data = ch.fault ? (data | 0x80) : (data & 0x7F); 
  //Serial.println(String(data));
  return data;
}

void BaseMCU::readVersion(void){

  if(readStart(BASEMCU_ADDR,VERSION,1)){ 
      
      if(I2C->available()){
        baseMCUVer = I2C->read(); 
      }      
  }
}

void BaseMCU::setUSB3Enable(bool set){
  int err=0;
  uint8_t data;
  if(initiated){  
    I2C->beginTransmission(BASEMCU_ADDR);
    I2C->write(MUXOECTR);    
    set ? data = 0x01 : data = 0x00;  
    I2C->write(data);  
  }
    err = I2C->endTransmission();
}
