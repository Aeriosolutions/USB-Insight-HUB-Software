//Add licence text
//PAC1943 definitions

#include <Arduino.h>
#include "PAC194x.h"

bool PAC194x::begin(TwoWire *theWire){

  int err = 0;
  int dat = 0;
  theWire->beginTransmission(PAC194x_ADDR);
  theWire->write(PAC194X_PRODUCT_ID_ADDR); //Product ID
  err = theWire->endTransmission(false);
  //delayMicroseconds(20);  
  theWire->requestFrom(PAC194x_ADDR,1);
  if(theWire->available())
  {
    if(theWire->read()==PAC1943_PRODUCT_ID) {
      I2C = theWire;
      initiated=true;
      //send configuration
      write16(PAC194X_CTRL_ADDR,CTRL_ADDR_L,CTRL_ADDR_H);
      write16(PAC194X_NEG_PWR_FSR_ADDR,NEG_PWR_ADDR_L,NEG_PWR_ADDR_H);
      //Configure PAC194X_SLOW_ADDR here if necessary
      //Configure PAC194X_ACCUM_CONFIG_ADDR, PAC194X_ACC_FULLNESS_LIMITS_ADDR, if necessary
      refresh();
      delayMicroseconds(1000);; //wait refresh completion; //wait refresh completion
      //write8(PAC194X_REFRESH_CMD_ADDR,1);
      enableAlerts(false);
      write8(PAC194X_OC_LIMIT_N_SAMPLES,OC_SAMPLES); //Set consecutive samples to trigger OC alert
      write8(PAC194X_UC_LIMIT_N_SAMPLES,UC_SAMPLES); //Set consecutive samples to trigger UC alert
      write24(PAC194X_SLOW_ALERT1_ADDR,0,0,ALERT1_INT_EN); //Alert1 interrupt sources


      for (int k=0; k<3; k++){
        setCurrentLimit(chMeterArr[k].fwdCLim,FORWARD,k); //reference value for test
        refresh_v();
        setCurrentLimit(chMeterArr[k].backCLim,BACKWARD,k); //reference value for test
        refresh_v();
      }
      
      enableAlerts(true);
      //refresh();
      delayMicroseconds(1000);; //wait refresh completion


      //initialize averager
      for (int i=0; i<3; i++)
      {
        chAverager[i].VoltageTotal=0;
        chAverager[i].CurrentTotal=0;
        chAverager[i].VoltageAveraged=0;
        chAverager[i].CurrentAveraged=0;

        for (int j=0; j<AVG_WINDOW_SIZE; j++)
        {
          chAverager[i].VoltageBuf[j]=0;
          chAverager[i].CurrentBuf[j]=0;
        }
      }

      return true;
    }
  }
  return false;
}

void PAC194x::write24(uint8_t reg_address,uint8_t lowByte, uint8_t midByte, uint8_t highByte){
  if(!initiated) return;
  int err = 0;
  I2C->beginTransmission(PAC194x_ADDR);      
  I2C->write(reg_address); 
  I2C->write(highByte);
  I2C->write(midByte);
  I2C->write(lowByte);
  err = I2C->endTransmission();   
}

void PAC194x::write16(uint8_t reg_address,uint8_t lowByte, uint8_t highByte){
  if(!initiated) return;
  int err = 0;
  I2C->beginTransmission(PAC194x_ADDR);      
  I2C->write(reg_address); 
  I2C->write(highByte);
  I2C->write(lowByte);
  err = I2C->endTransmission();   
}

void PAC194x::write8(uint8_t reg_address,uint8_t data){
  if(!initiated) return;
  int err = 0;
  I2C->beginTransmission(PAC194x_ADDR);      
  I2C->write(reg_address); 
  I2C->write(data);      
  err = I2C->endTransmission();   
}

void PAC194x::refresh_v(){
  //write8(PAC194X_REFRESH_V_CMD_ADDR,1);
  if(!initiated) return;
  int err = 0;
  I2C->beginTransmission(PAC194x_ADDR);      
  I2C->write(PAC194X_REFRESH_V_CMD_ADDR);     
  err = I2C->endTransmission();   
}

void PAC194x::refresh(){
  //write8(PAC194X_REFRESH_V_CMD_ADDR,1);
  if(!initiated) return;
  int err = 0;
  I2C->beginTransmission(PAC194x_ADDR);      
  I2C->write(PAC194X_REFRESH_CMD_ADDR);     
  err = I2C->endTransmission();   
}

void PAC194x::readAvgMeter(){

  if(!initiated) return;

  uint8_t lsb;
  uint8_t msb;

  int err=0;
  int i=0;
  uint16_t aux16=0;
  int sign=1;
  unsigned long i2cwd_timer = 0;

  I2C->beginTransmission(PAC194x_ADDR); 
  I2C->write(PAC194X_VBUS1_AVG_ADDR); 
  err = I2C->endTransmission(false);
  i2cwd_timer = millis(); //*probably this protection is not longer necessary
  err = I2C->requestFrom(PAC194x_ADDR,12); //*
  if(millis()-i2cwd_timer > 2){ //*workaround for sudden drop in I2C speed, reported here https://github.com/espressif/arduino-esp32/issues/8480
    Serial.println("I2C Slow down on PAC detected!!"); //*
    I2C->flush(); //*
    I2C->setClock(100000); //*
    I2C->setClock(400000); //*
    return; //*
  }
  if(err==0) {
    Serial.println("PAC Fail to read bytes");
    return;
  }
  for(i =0; i < 3; i++)
  {
    msb=I2C->read();
    lsb=I2C->read();
    chMeterArr[i].AvgVoltageRaw = msb*256 + lsb;
    chMeterArr[i].AvgVoltage = ((float)(chMeterArr[i].AvgVoltageRaw)/65536.0)*9000.0; //in mV

    chAverager[i].VoltageTotal=chAverager[i].VoltageTotal-chAverager[i].VoltageBuf[filterIndex]; // Remove the oldest entry from the sum
    chAverager[i].VoltageBuf[filterIndex] = chMeterArr[i].AvgVoltage; // Add the newest reading to the window
    chAverager[i].VoltageTotal = chAverager[i].VoltageTotal + chMeterArr[i].AvgVoltage; // Add the newest reading to the sum
    chAverager[i].VoltageAveraged = chAverager[i].VoltageTotal/AVG_WINDOW_SIZE;
  }
  
  for(i =0; i < 3; i++)
  {
    msb=I2C->read();
    lsb=I2C->read();
    chMeterArr[i].AvgVsenseRaw = msb*256 + lsb;
    if(chMeterArr[i].AvgVsenseRaw & 0x8000){
      aux16 = (uint16_t)(~(chMeterArr[i].AvgVsenseRaw-1));
      sign=1;  //sign inverted due to hardware connection
    }
    else {
      aux16 = chMeterArr[i].AvgVsenseRaw;
      sign=-1;
    }

    chMeterArr[i].AvgCurrent = sign*((float)(aux16)/32768.0)*chMeterArr[i].FullScale;  //in mA

    chAverager[i].CurrentTotal=chAverager[i].CurrentTotal-chAverager[i].CurrentBuf[filterIndex]; // Remove the oldest entry from the sum
    chAverager[i].CurrentBuf[filterIndex] = chMeterArr[i].AvgCurrent; // Add the newest reading to the window
    chAverager[i].CurrentTotal = chAverager[i].CurrentTotal + chMeterArr[i].AvgCurrent; // Add the newest reading to the sum
    chAverager[i].CurrentAveraged = chAverager[i].CurrentTotal/AVG_WINDOW_SIZE;

  }
  
  filterIndex = (filterIndex+1) % AVG_WINDOW_SIZE;
}

void PAC194x::setCurrentLimit(float climit, bool cdir, int ch){
  uint16_t aux16 = 0;
  if(!initiated || ch>=3 ) return;
  //alerts must be disabled before changing OC/UC limits according to PAC datasheet
  //enableAlerts(false);
  //Note that the current circulation is reversed: negative values are forward and positive values backwards
  //aux16 = (uint16_t)(round(climit/chMeterArr[ch].FullScale*32767)); //use for -100/+100mV FSR - convert the desired current limit to 16bit Hex
  aux16 = (uint16_t)(round(climit/chMeterArr[ch].FullScale*16383)); //use for -50/+50mV FSR - convert the desired current limit to 16bit Hex
  //Serial.println(String(aux16));

  if(cdir){
    //Configure undercurrent limits  
    //aux16 = (~aux16 +1) | 0x8000 ; //convert to two's compliment as is negative number
    aux16 = ~aux16 +1; //convert to two's compliment as is negative number
    //Serial.println(String(aux16));
    write16(PAC194X_UC_LIMIT1_ADDR+ch,uint8_t(aux16),uint8_t(aux16>>8));
    
  } 
  else {
    //Configure overcurrent limits
    
    write16(PAC194X_OC_LIMIT1_ADDR+ch,uint8_t(aux16),uint8_t(aux16>>8));

  }

  //enableAlerts(true);
  return;

}

void PAC194x::enableAlerts(bool enable){
  
  if(!initiated) return;

  if(enable){
    write24(PAC194X_ALERT_ENABLE_ADDR,0,0,ENABLEALERT);
  }
  else {
    write24(PAC194X_ALERT_ENABLE_ADDR,0,0,DISABLEALERT);
  }
  refresh_v();
}

uint8_t PAC194x::readInterruptFlags(){

  //This function is used by an interrupt so it must be short as possible
  //Updating of meter.fwdAlertSet and backAlertSet flags should be updated in the calling interrupt
  uint8_t flags;
  uint8_t other;
  if(!initiated) return 0; 
  I2C->beginTransmission(PAC194x_ADDR);  
  I2C->write(PAC194X_ALERT_STATUS_ADDR); //Alert Status
  int err = I2C->endTransmission(false);
  //delayMicroseconds(20);  
  err=I2C->requestFrom(PAC194x_ADDR,3);  
  //OC and UC flags are in the first byte. Two last bytes are discarded
  flags=I2C->read();
  for (int i=0; i<2; i++) other=I2C->read();
  return flags; 

}

uint16_t PAC194x::read16(uint8_t address){
  uint16_t val;
  if(!initiated) return 0;
  I2C->beginTransmission(PAC194x_ADDR);
  I2C->write(address); //Alert Status
  int err = I2C->endTransmission(false);
  I2C->requestFrom(PAC194x_ADDR,2);
  uint8_t msb=I2C->read();
  uint8_t lsb=I2C->read();

  val = (uint16_t)(msb*256 + lsb);
  return val;
}

