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

//PAC1943 non/comprehensive driver. Implements only required functions for the USB Insight Hub project.

#ifndef PAC194X_H
#define PAC194X_H

#include <Arduino.h>
#include <Wire.h>

#define SLOWDOWN_TIMEOUT 4

#define PAC194x_ADDR 0x10 //set in the base board by a resistor 
#define PAC1943_PRODUCT_ID 0x6A //for PAC1943 (3 channels)

#define FULLSCALE_15mOHM 3333.333 //Full scale is 50mV/0.015
#define FULLSCALE_20mOHM 2500 //Full scale is 50mV/0.020
#define FULLSCALE_10mOHM 5000 //Full scale is 50mV/0.010

#define FILTER_TYPE_MOVING_AVG 0
#define FILTER_TYPE_MEDIAN 1
#define MAX_FILTER_WINDOW_SIZE 20

//define configurations
//Configuration control for enabling bidirectional current and bipolar voltage measurements Page 52
#define NEG_PWR_ADDR_L 0x00//00 00 00 00 -> 01 VBUS unipolar 0V to 9V FSR
#define NEG_PWR_ADDR_H 0xAA//10 10 10 10 -> 10 VSENSE bipolar -50mV to +50mV; 
//Configuration controls and status Pag 45
#define CTRL_ADDR_L 0x10//0001 0000 -> high nible disabled channels (CH4 disabled); low nibble all 0s
#define CTRL_ADDR_H 0x00// 0000->1024 samples adaptive accumulation |00-> ALERT2 as ALERT |00->ALERT as ALERT1

//Software default current limits
#define DEFAULT_FWD_C_LIM 2000.0 //this value is for a configuration of -50/+50 mV FSR
#define DEFAULT_BACK_C_LIM 20.0 //this value is for a configuration of -50/+50 mV FSR
//These values must be higher than the deglitch timers of the power switches AP22653A (typ 6ms)
#define UC_SAMPLES 0XA8 // 10 10 10 00 -> 10 = 8 samples ~ 8ms FORWARD 
#define OC_SAMPLES 0XA8 // 10 10 10 00 -> 10 = 8 samples ~ 8ms BACKWARD 
//Alerts configuration
#define ENABLEALERT 0xEE 
#define DISABLEALERT 0x00
#define ALERT1_INT_EN 0xEE 

//Current limit direction
#define FORWARD true
#define BACKWARD false

//SMBUS_SETTINGS_ADDR as default

//defines copied from the official Microchip drivers
#define PAC194X_REFRESH_CMD_ADDR            0x00
#define PAC194X_CTRL_ADDR                   0x01
#define PAC194X_ACC_COUNT_ADDR              0x02
#define PAC194X_VACC1_ADDR                  0x03
#define PAC194X_VACC2_ADDR                  0x04
#define PAC194X_VACC3_ADDR                  0x05
#define PAC194X_VACC4_ADDR                  0x06
#define PAC194X_VBUS1_ADDR                  0x07
#define PAC194X_VBUS2_ADDR                  0x08
#define PAC194X_VBUS3_ADDR                  0x09
#define PAC194X_VBUS4_ADDR                  0x0A
#define PAC194X_VSENSE1_ADDR                0x0B
#define PAC194X_VSENSE2_ADDR                0x0C
#define PAC194X_VSENSE3_ADDR                0x0D
#define PAC194X_VSENSE4_ADDR                0x0E
#define PAC194X_VBUS1_AVG_ADDR              0x0F
#define PAC194X_VBUS2_AVG_ADDR              0x10
#define PAC194X_VBUS3_AVG_ADDR              0x11
#define PAC194X_VBUS4_AVG_ADDR              0x12
#define PAC194X_VSENSE1_AVG_ADDR            0x13
#define PAC194X_VSENSE2_AVG_ADDR            0x14
#define PAC194X_VSENSE3_AVG_ADDR            0x15
#define PAC194X_VSENSE4_AVG_ADDR            0x16
#define PAC194X_VPOWER1_ADDR                0x17
#define PAC194X_VPOWER2_ADDR                0x18
#define PAC194X_VPOWER3_ADDR                0x19
#define PAC194X_VPOWER4_ADDR                0x1A
#define PAC194X_SMBUS_SETTINGS_ADDR         0x1C
#define PAC194X_NEG_PWR_FSR_ADDR            0x1D
#define PAC194X_REFRESH_G_CMD_ADDR          0x1E
#define PAC194X_REFRESH_V_CMD_ADDR          0x1F
#define PAC194X_SLOW_ADDR                   0x20
#define PAC194X_CTRL_ACT_ADDR               0x21
#define PAC194X_NEG_PWR_FSR_ACT_ADDR        0x22
#define PAC194X_CTRL_LAT_ADDR               0x23
#define PAC194X_NEG_PWR_FSR_LAT_ADDR        0x24
#define PAC194X_ACCUM_CONFIG_ADDR           0x25
#define PAC194X_ALERT_STATUS_ADDR           0x26
#define PAC194X_SLOW_ALERT1_ADDR            0x27
#define PAC194X_GPIO_ALERT2_ADDR            0x28
#define PAC194X_ACC_FULLNESS_LIMITS_ADDR    0x29
#define PAC194X_OC_LIMIT1_ADDR              0x30
#define PAC194X_OC_LIMIT2_ADDR              0x31
#define PAC194X_OC_LIMIT3_ADDR              0x32
#define PAC194X_OC_LIMIT4_ADDR              0x33
#define PAC194X_UC_LIMIT1_ADDR              0x34
#define PAC194X_UC_LIMIT2_ADDR              0x35
#define PAC194X_UC_LIMIT3_ADDR              0x36
#define PAC194X_UC_LIMIT4_ADDR              0x37
#define PAC194X_OP_LIMIT1_ADDR              0x38
#define PAC194X_OP_LIMIT2_ADDR              0x39
#define PAC194X_OP_LIMIT3_ADDR              0x3A
#define PAC194X_OP_LIMIT4_ADDR              0x3B
#define PAC194X_OV_LIMIT1_ADDR              0x3C
#define PAC194X_OV_LIMIT2_ADDR              0x3D
#define PAC194X_OV_LIMIT3_ADDR              0x3E
#define PAC194X_OV_LIMIT4_ADDR              0x3F
#define PAC194X_UV_LIMIT1_ADDR              0x40
#define PAC194X_UV_LIMIT2_ADDR              0x41
#define PAC194X_UV_LIMIT3_ADDR              0x42
#define PAC194X_UV_LIMIT4_ADDR              0x43
#define PAC194X_OC_LIMIT_N_SAMPLES          0x44
#define PAC194X_UC_LIMIT_N_SAMPLES          0x45
#define PAC194X_OP_LIMIT_N_SAMPLES          0x46
#define PAC194X_OV_LIMIT_N_SAMPLES          0x47
#define PAC194X_UV_LIMIT_N_SAMPLES          0x48    
#define PAC194X_ALERT_ENABLE_ADDR           0x49
#define PAC194X_ACCUM_CONFIG_ACT_ADDR       0x4A
#define PAC194X_ACCUM_CONFIG_LAT_ADDR       0x4B

#define PAC194X_PRODUCT_ID_ADDR             0xFD
#define PAC194X_MANUFACTURER_ID_ADDR        0xFE
#define PAC194X_REVISION_ID_ADDR            0xFF

struct meter {
  uint32_t FullScale;
  float fwdCLim;
  float backCLim;  
  uint16_t AvgVoltageRaw;
  uint16_t AvgVsenseRaw;
  float AvgVoltage;
  float AvgCurrent;
  bool fwdAlertSet;
  bool backAlertSet;
  int filterType;
};

struct meter_averager {
  float VoltageBuf[MAX_FILTER_WINDOW_SIZE];
  float VoltageTotal;
  float VoltageAveraged;
  float CurrentBuf[MAX_FILTER_WINDOW_SIZE];
  float CurrentTotal;
  float CurrentAveraged;
};


class PAC194x {

  public:
    //These values should be initialized when the class is created as some of them may be in non-volatile storage
    meter chMeterArr[3]={ {FULLSCALE_20mOHM,DEFAULT_FWD_C_LIM,DEFAULT_BACK_C_LIM,0,0,0,0,false,false,FILTER_TYPE_MEDIAN},
                          {FULLSCALE_20mOHM,DEFAULT_FWD_C_LIM,DEFAULT_BACK_C_LIM,0,0,0,0,false,false,FILTER_TYPE_MEDIAN},
                          {FULLSCALE_20mOHM,DEFAULT_FWD_C_LIM,DEFAULT_BACK_C_LIM,0,0,0,0,false,false,FILTER_TYPE_MEDIAN}
                        };

    meter_averager chAverager[3];
    bool begin(TwoWire *theWire);    
    void refresh_v();
    void refresh();
    void readAvgMeter();
    void setCurrentLimit(float climit, bool cdir, int ch);
    void setFilterLength(uint8_t length);
    void enableAlerts(bool enable);
    //bool readAndClearPORFlag();
    uint8_t readInterruptFlags();
    uint16_t read16(uint8_t address);
    bool isInitiated() { return initiated; }   


  private:
    uint8_t filterWindowsize = 10;
    void write24(uint8_t reg_address,uint8_t lowByte, uint8_t midByte, uint8_t highByte);
    void write16(uint8_t reg_address,uint8_t lowByte, uint8_t highByte);
    void write8(uint8_t reg_address,uint8_t data);
    void voltageMovingAverageFilter(int i);
    void currentMovingAverageFilter(int i);
    void voltageMedianFilter(int i);
    void currentMedianFilter(int i);
    float medianFunction(float arr[], int n);
    float movingAverage(float arr[], int n);

    TwoWire *I2C;
    bool initiated = false;
    int filterIndex = 0;

};

#endif //PAC194X_H