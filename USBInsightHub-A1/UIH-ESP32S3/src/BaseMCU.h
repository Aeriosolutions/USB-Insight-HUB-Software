//Add licence text
#ifndef BASEMCU_H
#define BASEMCU_H

#include <Arduino.h>
#include <Wire.h>

#define BASEMCU_ADDR 0x51
#define WHOAMI_ID    0x35

#define SLOWDOWN_TIMEOUT 3

//BaseMCU register addresses
#define WHOAMI   0x10  //BASE MCU ID
#define VERSION  0x12  //VERSION 
#define CH1REG   0x20  //CH1 Fault, Power enable and current settings
#define CH2REG   0x21  //CH2 Fault, Power enable and current settings
#define CH3REG   0x22  //CH3 Fault, Power enable and current settings
#define CCSUM    0x23  //VEXT and VOST CC lines summary
#define AUXREG   0x24  //USB3 and Power Muxer values
#define MUXOECTR 0x26  //Control muxer signal enable to work as USB 2.0 or USB 3.0 Hub
#define VEXTCC1  0x30  //VEXT CC1 voltage reading: 30-L byte 31-H byte
#define VEXTCC2  0x32  //VEXT CC2 voltage reading: 32-L byte 33-H byte
#define VHOSTCC1 0x34  //VHOST CC1 voltage reading: 34-L byte 35-H byte
#define VHOSTCC2 0x36  //VHOST CC2 voltage reading: 36-L byte 37-H byte

//Current limit definition on CHREG
#define ILIM_0_5 0
#define ILIM_1_0 1
#define ILIM_1_5 2
#define ILIM_2_0 3

//CSUM Definitions - Current limit given by CC pull-up values.
#define UNKNOWNPWR 0  //Unknown - No pull up
#define DEFAULTPWR 1  //56k +/- 20%
#define PWR_1_5    2  //22k +/- 5%
#define PWR_3_0    3  //10k +/- 5%

#define NOPULLUP   0  //CC1 nor CC2 has pull-up
#define CC1PULLUP  1  //CC1 has the pull-up
#define CC2PULLUP  2  //CC2 has the pull-up
#define CCERRSTATE 3  //CC1 and CC2 are pulled up at the same time or CC1 or CC2 voltages are out of range


struct Channel {
  bool fault;
  bool data_en;
  bool pwr_en;
  uint8_t ilim;
};

class BaseMCU {
  public:
    bool begin(TwoWire *theWire);
    void readAll();
    void writeAll();
    void readVersion();
    void setUSB3Enable(bool set);
    bool initiated = false;

    Channel chArr[3]={{false, false,false,ILIM_0_5},{false, false,false,ILIM_0_5},{false, false,false,ILIM_0_5}};
    uint8_t vextCC = UNKNOWNPWR;
    uint8_t extState = NOPULLUP;
    uint8_t vhostCC = UNKNOWNPWR;
    uint8_t hostState =  NOPULLUP;
    uint8_t baseMCUVer = 0;

    bool pwrsource = false; //false vhost used, true vext used
    bool muxoe = false;     //false disabled, true enabled
    bool muxsel = false;    //false pos 1, true pos 2  
  
  private: 
    TwoWire *I2C;
    Channel parseCHREG(uint8_t data);
    uint8_t encodeCHREG(Channel ch);
    bool readStart(int address, int start, int numBytes);
};

#endif //BASEMCU