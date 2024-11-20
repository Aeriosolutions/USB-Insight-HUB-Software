//Add licence text

//Enviroment
//esp32 arduino version: 2.0.14
//TFT_eSPI version: 2.5.43
//USB CDC on Boot: Disabled
//USB DFU on Boot: Disabled
//USB Mode: "USB-OTG (Tiny USB)"

/*
Test routines for the USBy hardware 0 board.
v0.0.0 06/10/2023 First code
v0.0.1 06/11/2023 Change button logic and data switching logic

Move to PCB A0 board and use ESP32-S2
v1.0.0 05/03/2024 Base on 0 board, but lot of changes to migrate to A0

Move to PCB board B0
v2.0.0 22/06/2024 Based on A0 board, add additional setup button.

*/

#include <Arduino.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include "Buttons.h"
#include "BaseMCU.h"
#include "Screen.h"
#include "PAC194x.h"
#include "USB.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "Web.h"


//#define A0 1
#define B0 1

//Pin definitions
#define VBUS_MONITOR 1
//Display pins definitions in Screen.h library
#define STEMMA_SCL  7
#define STEMMA_SDA  8
#define HUB_RESET  21
#define HUB_PGANG  26 //if ESP32-S2 modules with PSRAM are used, this must be unused
#define B2B_SCL    33
#define B2B_SDA    34
#define PAC_ALERT  35
#define BUTTON_3   37
#define BUTTON_2   38
#define BUTTON_1   39
#define AUX_LED    45

#ifdef A0 //Board A0
  #define ESP_USB_SW_EN 18
  #define SETUP         36
#endif 

#ifdef B0 //Board B0
  #define ESP_USB_SW_EN 46 //connected to TP11, signal not used
  //#define SETUP       36 //lateral button
  #define SETUP         18 //frontal button
#endif 


//Task looping variables
#define HEART_BEAT_PERIOD             500 //in ms
#define SERIAL_PERIOD                  50 //in ms
#define BUTTON_CHECK_PERIOD            40 //in ms
#define BMCU_I2C_SCAN_PERIOD           40 //in ms
#define METER_I2C_SCAN_PERIOD          40 //in ms
#define DISPLAY_REFRESH_PERIOD         40 //note that for each screen the effective rate is 120ms
#define METER_DISPLAY_UPDATE_PERIOD   500
#define PC_CONNECTION_TIMEOUT        2500 
#define WEB_REFRESH_PERIOD            500 //in ms
#define STARTUP_TIMER_PERIOD          100 //ms

//Error definitions
#define VERSION_ERROR     1
#define CONN_WIFI_ERROR   2
#define SPIFF_ERROR       3
#define FILE_RW_ERROR     4
#define CONN_INFO_ERROR   5

unsigned long heart_beat_timer           = 0;
unsigned long serial_timer               = 0;
unsigned long button_check_timer         = 0;
unsigned long bmcu_i2c_scan_timer        = 0;
unsigned long meter_i2c_scan_timer       = 0;
unsigned long display_refresh_timer      = 0;
unsigned long meter_display_update_timer = 0;
unsigned long web_refresh_timer          = 0;
unsigned long startup_timer              = 0;

//I2C Instances
TwoWire I2CB2B = TwoWire(0);
BaseMCU bMCU;
PAC194x bMeter; 
volatile bool i2c_free = true; //volatile to be used inside interrupts

chScreenData ScreenArr[3];

Screen iScreen;
int iScnt=0;
int testcnt = 0;


static ChannelData channel1 = {Channel_1, "COM2", 5.001, 0.250, 1000, -20, false, false};
static ChannelData channel2 = {Channel_2, "COM7", 4.902, 1.523, 1900, -20, true, false};
static ChannelData channel3 = {Channel_3, "COM3", 5.025, 0.000, 500, -10, false, true};

struct chWebData {  
  int overCurrentLimit;
  int backCurrentLimit;
  bool powerControl;
  bool dataControl;
} LastWebStateArr[3];

//Button variables and Interupt functions
Button ButtonArray[3] = { {BUTTON_1,false,true,0,0,0,0,false},
                          {BUTTON_2,false,true,0,0,0,0,false},
                          {BUTTON_3,false,true,0,0,0,0,false}
                        };

Button Setup_Button = {SETUP,false,true,0,0,0,0,false};

//Starup sequence values
bool startupactive = true;
//int StartUpTimer[3] = {41,61,81}; //delay in 0.1s, triggers at 1. 0 is already active, -1 is off
int StartUpTimer[3] = {1,1,1}; //delay in 0.1s, triggers at 1. 0 is already active, -1 is off
int StartUpCounter[3];

//Route the power meter physical channels to the board channels
uint8_t meterMap[3]={2,0,1}; 
//Route the board channels to power meter physical channels
uint8_t invmeterMap[3]={1,2,0}; 


void IRAM_ATTR ch1_btn_isr() { btnDebounce(&ButtonArray[0]); }
void IRAM_ATTR ch2_btn_isr() { btnDebounce(&ButtonArray[1]); }
void IRAM_ATTR ch3_btn_isr() { btnDebounce(&ButtonArray[2]); }
void IRAM_ATTR setup_btn_isr() { btnDebounce(&Setup_Button); }
void IRAM_ATTR pac_alert_isr(){ pacAlertInterruptHandler(); }

//FreeRTOS globals
TaskHandle_t pac_alert_handle;
TaskHandle_t start_up_handle;
SemaphoreHandle_t i2c_Semaphore;


//USB Serial and Harware Serial (Debug)
#if ARDUINO_USB_CDC_ON_BOOT
#define HWSerial Serial0
#define USBSerial Serial
#else
#define HWSerial Serial
USBCDC USBSerial;
#endif
bool USBSerialActivity = false;

//Port values from pc strings
char *pcstrings[13]={"PC","0","-","-","0","0","-","-","0","0","-","-","0"}; //array of pointers to each value
/*{"PC",<CH1_NUM_OF_DEVICES>,<CH1_USB_DEV_1>,<CH1_USB_DEV_2>,<CH1_USB_TYPE>
        <CH2_NUM_OF_DEVICES>,<CH2_USB_DEV_1>,<CH2_USB_DEV_2>,<CH2_USB_TYPE>
        <CH3_NUM_OF_DEVICES>,<CH3_USB_DEV_1>,<CH3_USB_DEV_2>,<CH3_USB_TYPE>
}*/
char *pcptr = NULL;
char pcsbuffer[60]; //working array
String tempString;
unsigned long lastPCcom;

//Screen demo values
bool demoIsActive = false;
char *demostrings[13]={"PC","1","D:","-","3","2","COM29","COM30","2","0","-","-","0"};

void setup() {
  //Digital IO Initialization
  pinMode(ESP_USB_SW_EN,OUTPUT);
  pinMode(HUB_RESET,OUTPUT);
  pinMode(AUX_LED,OUTPUT);
  //pinMode(HUB_PGANG,INPUT); //if ESP32-S2 modules with PSRAM are used, this must be unused
  pinMode(PAC_ALERT,INPUT_PULLUP);
  pinMode(SETUP,INPUT_PULLUP);
  pinMode(BUTTON_1,INPUT_PULLUP);
  pinMode(BUTTON_2,INPUT_PULLUP);
  pinMode(BUTTON_3,INPUT_PULLUP);
  //Screen pins configured in Screen -> start

  //Reset Hub Chip
  digitalWrite(HUB_RESET,LOW);

  digitalWrite(ESP_USB_SW_EN,LOW);

  //Hardware Serial Ini
  HWSerial.begin(115200); //USB HWSerial to conect to PC
  //HWSerial.setTimeout(50);
  //HWSerial.setDebugOutput(true);

  //USB Configuration
  USB.onEvent(usbEventCallback);
  USBSerial.onEvent(usbEventCallback);
  
  USBSerial.begin();
  USB.productName("InsightHUB Controller");
  USB.begin();


  //I2C initialization
  i2c_Semaphore = xSemaphoreCreateMutex();
  if(i2c_Semaphore == NULL) HWSerial.println("I2C Semaphore Creation failed");

  if(xSemaphoreTake(i2c_Semaphore,( TickType_t ) 10 ) == pdTRUE)
  {
    I2CB2B.begin(B2B_SDA, B2B_SCL, 400000);
    I2CB2B.setTimeOut(20);
        
    delay(20);
    if(bMCU.begin(&I2CB2B)) HWSerial.println("Base MCU initialized OK");
    delay(30); //required time after powerup to write to meter
    if(bMeter.begin(&I2CB2B)) HWSerial.println("Power meter initialized OK");
    
    xSemaphoreGive(i2c_Semaphore);
  } 
  else {
    HWSerial.println("I2C Hardware is bussy, could not initialize I2C peripherals");
  }


  //Interrupts registration
  attachInterrupt(ButtonArray[0].PIN, ch1_btn_isr, FALLING);
  attachInterrupt(ButtonArray[1].PIN, ch2_btn_isr, FALLING);
  attachInterrupt(ButtonArray[2].PIN, ch3_btn_isr, FALLING);
  attachInterrupt(Setup_Button.PIN, setup_btn_isr, FALLING);
  attachInterrupt(PAC_ALERT, pac_alert_isr, FALLING);
  
  //Display Initial values

  ScreenArr[0].dProp = {DISPLAY_CS_1, DLIT_1, 2, 800};
  ScreenArr[1].dProp = {DISPLAY_CS_2, DLIT_2, 2, 800};
  ScreenArr[2].dProp = {DISPLAY_CS_3, DLIT_3, 2, 800};  

  for (int i=0; i<3; i++) {
    StartUpCounter[i]=StartUpTimer[i];
  }  

  for (int i=0; i<3; i++) {   
    ScreenArr[i].mProp = {0,0,bMeter.chMeterArr[i].fwdCLim,bMeter.chMeterArr[i].backCLim,bMeter.chMeterArr[i].fwdAlertSet,bMeter.chMeterArr[i].backAlertSet};
    ScreenArr[i].tProp = {0,"-","-",0};
    ScreenArr[i].fault = false;
    ScreenArr[i].data_en = true;
    ScreenArr[i].pwr_en = true;
    ScreenArr[i].ilim = 3; 
    ScreenArr[i].pconnected = false;
    ScreenArr[i].startup_timer = StartUpTimer[i];
    ScreenArr[i].startup_cnt = StartUpCounter[i];
  }  
 
  //for tests
  for (int i=0; i<3; i++) {
    bMCU.chArr[i].pwr_en = false;
    bMCU.chArr[i].data_en = true;
    bMCU.chArr[i].ilim = 3;
  } 
  mcuWriteAll();

  //display initialization
  iScreen.start();
  for (int i=0; i<3; i++) {
    iScreen.updateScreen(ScreenArr[i]);
  }
  analogWrite(DLIT_1,800);
  analogWrite(DLIT_2,800);
  analogWrite(DLIT_3,800);

  StartWifi();
  StartWebView();
  
  //Task initialization
  xTaskCreate(pac_alert_handler_task, "pac alert handler", 2048,  NULL, 2,  &pac_alert_handle );
  xTaskCreate(start_up_handler_task, "start_up_handler_task", 2048,  NULL, 5,  &start_up_handle );
  //xTaskCreatePinnedToCore(start_up_handler_task, "start_up_handler_task", 2048,  NULL, 5,  &start_up_handle,1);
}

void loop() {
  
  // heart beat used only if led is soldered
  if(millis()-heart_beat_timer >= HEART_BEAT_PERIOD)
  {
    heart_beat_timer = millis();
    //digitalWrite(AUX_LED, !digitalRead(AUX_LED));
    testcnt++;
    if(testcnt>100) testcnt=0;
    //if(digitalRead(HUB_PGANG)) ScreenArr[0].hub_suspend = true; else ScreenArr[0].hub_suspend = false;    
  }

  
  //serial loop - check if there has been serial activity to update the pc-connection status icon 
  if(millis()-serial_timer >= SERIAL_PERIOD)
  {
    serial_timer = millis();
    if(USBSerialActivity){

      lastPCcom = serial_timer;
      for (int i=0; i<3 ; i++) ScreenArr[i].pconnected = true;
      USBSerialActivity=false;
    }

    if(serial_timer-lastPCcom > PC_CONNECTION_TIMEOUT){
      for (int i=0; i<3 ; i++)
        ScreenArr[i].pconnected = false;
    }

  }

  //I2C scan for Base MCU
  if(millis()-bmcu_i2c_scan_timer >= BMCU_I2C_SCAN_PERIOD)
  {
    bmcu_i2c_scan_timer = millis();
    
    if(i2c_Semaphore!=NULL){
      if(xSemaphoreTake(i2c_Semaphore,portMAX_DELAY) == pdTRUE){
        bMCU.readAll();       
        xSemaphoreGive(i2c_Semaphore);
      } 
      else{
        HWSerial.println("Timeout to read I2C mcu");
      }
    }

    for (int i=0; i<3 ; i++){
      ScreenArr[i].fault   = bMCU.chArr[i].fault;
      ScreenArr[i].data_en = bMCU.chArr[i].data_en;
      ScreenArr[i].pwr_en  = bMCU.chArr[i].pwr_en;
      ScreenArr[i].ilim    = bMCU.chArr[i].ilim;
      ScreenArr[i].mProp.fwdAlertSet  = bMeter.chMeterArr[i].fwdAlertSet;
      ScreenArr[i].mProp.backAlertSet = bMeter.chMeterArr[i].backAlertSet;
      ScreenArr[i].mProp.fwdCLim      = bMeter.chMeterArr[i].fwdCLim;
      ScreenArr[i].mProp.backCLim     = bMeter.chMeterArr[i].backCLim;            
    }

  } 

  //I2C read of the power meter PAC1943
  if(millis()-meter_i2c_scan_timer >= METER_I2C_SCAN_PERIOD)
  { 
    meter_i2c_scan_timer = millis();

    if(i2c_Semaphore != NULL){
      if(xSemaphoreTake(i2c_Semaphore,portMAX_DELAY) == pdTRUE){
        bMeter.refresh_v();
        delayMicroseconds(500); //required to update Meter registers after refresh command. 
        bMeter.readAvgMeter();        
        xSemaphoreGive(i2c_Semaphore);
      } 
      else{
        HWSerial.println("Timeout to get access to I2C read meter");
      }
    }                  
    
  }

  //update the values of the mater to be displayed
  if(millis()-meter_display_update_timer >= METER_DISPLAY_UPDATE_PERIOD)
  {    
    meter_display_update_timer = millis();
         
    for (int i=0; i<3 ; i++){
      ScreenArr[i].mProp.AvgCurrent = bMeter.chAverager[meterMap[i]].CurrentAveraged;
      ScreenArr[i].mProp.AvgVoltage = bMeter.chAverager[meterMap[i]].VoltageAveraged;
      //ScreenArr[i].mProp.AvgCurrent=bMeter.chMeterArr[meterMap[i]].AvgCurrent;
      //ScreenArr[i].mProp.AvgVoltage=bMeter.chMeterArr[meterMap[i]].AvgVoltage;      
    }         
  }


  //button scan and logic
  if(millis()-button_check_timer >= BUTTON_CHECK_PERIOD)
  {
    button_check_timer = millis();
    int tot = 0;
    for (int i=0; i<3; i++){
      tot=resolveButton(&ButtonArray[i]);
      if (tot==1) {
        HWSerial.println("Button "+ String(i+1) + " short pressed");        
        if(StartUpCounter[i]>0){ //If button pressed during startup timer, cancel the timer and leave off
          StartUpCounter[i]=-1;
          HWSerial.println("Cancel Start up timer");
        }
        else {
          if(bMeter.chMeterArr[i].backAlertSet || bMeter.chMeterArr[i].fwdAlertSet){
            HWSerial.println("Reset OC and BC");
            bMeter.chMeterArr[i].backAlertSet = false;
            bMeter.chMeterArr[i].fwdAlertSet  = false;
          }
          else
          {
            bMCU.chArr[i].pwr_en = !bMCU.chArr[i].pwr_en;
            mcuWriteAll();
          }
        }

      }
      if (tot==2) {
        HWSerial.println("Button " + String(i+1) +" long pressed");
        bMCU.chArr[i].data_en = !bMCU.chArr[i].data_en;
        mcuWriteAll();        
      }
    }

    tot=resolveButton(&Setup_Button);
    if (tot==1) {
      HWSerial.println("Setup button short press");
      float oclimit=2000;
      for (int i=0; i<3 ; i++){
        switch(ScreenArr[i].ilim){
            case 0: 
              bMCU.chArr[i].ilim = 1;
              oclimit=1000;
              break;
            case 1: 
              bMCU.chArr[i].ilim = 2;
              oclimit=1500;
              break;
            case 2: 
              bMCU.chArr[i].ilim = 3;
              oclimit=2000;
              break;
            case 3: 
              bMCU.chArr[i].ilim = 0;
              oclimit=500;
              break;              
            default:
              break;
        }
      } 
      mcuWriteAll();
      setCurrentLimitAll(oclimit);

    }
    if (tot==2){
      HWSerial.println("Setup button long press");
      /*if(demoIsActive) {
        demoIsActive= false;
        parseDataPC();
      }
      else demoIsActive= true;
      mcuWriteAll(); */
      //To change the screen rotation

      for(int i=0;i<3;i++){
        switch(ScreenArr[i].dProp.rotation){
            case 0: 
              ScreenArr[i].dProp.rotation = 1;
              break;
            case 1: 
              ScreenArr[i].dProp.rotation = 2;
              break;
            case 2: 
              ScreenArr[i].dProp.rotation = 3;
              break;
            case 3: 
              ScreenArr[i].dProp.rotation = 0;
              break;
        }                  
      }       
    } 
  }



  //screen update - one screen at a time is updated in every round-robin cycle  
  if(millis()-display_refresh_timer >= DISPLAY_REFRESH_PERIOD)
  {
    display_refresh_timer = millis();
    if(iScnt == 0) {
      ScreenArr[0].dProp.brightness=testcnt;
      ScreenArr[1].dProp.brightness=testcnt;
      ScreenArr[2].dProp.brightness=testcnt;
    }

    //demo screen ilumination
    if(demoIsActive){

      for(int i=0;i<3;i++){
        ScreenArr[i].tProp.numDev    = String(demostrings[4*i+1]).toInt();
        ScreenArr[i].tProp.Dev1_Name = String(demostrings[4*i+2]);
        ScreenArr[i].tProp.Dev2_Name = String(demostrings[4*i+3]);
        ScreenArr[i].tProp.usbType   = String(demostrings[4*i+4]).toInt();
        ScreenArr[i].pconnected      = true;
      }

      ScreenArr[0].pwr_en = true;
      ScreenArr[1].pwr_en = true;
      ScreenArr[2].pwr_en = false;

      ScreenArr[0].ilim = 0;
      ScreenArr[1].ilim = 3;
      ScreenArr[2].ilim = 2;


      ScreenArr[0].mProp.AvgVoltage = 5002;
      ScreenArr[0].mProp.AvgCurrent = 235;
      ScreenArr[1].mProp.AvgVoltage = 4920;
      ScreenArr[1].mProp.AvgCurrent = 1350;
      ScreenArr[2].mProp.AvgVoltage = 0;
      ScreenArr[2].mProp.AvgCurrent = 0;
    }

    iScreen.updateScreen(ScreenArr[iScnt]);
    iScnt++;
    if(iScnt==3) iScnt=0;

  }

  if(millis()-web_refresh_timer >= WEB_REFRESH_PERIOD)
  {
    //For buttons in the interface
    //First I read the status in the web interface
    for (int i=0; i<3 ; i++){
      LastWebStateArr[i].powerControl = getChannelPowerControl((eChannel)(i+1));      
    }
    //HWSerial.println("Web power state: "+ String(LastWebStateArr[0].powerControl) + " " + String(LastWebStateArr[1].powerControl) + " "+ String(LastWebStateArr[2].powerControl));
    
    /*for (int i=0; i<3 ; i++){      
      setChannelVoltage((eChannel)(i+1),bMeter.chAverager[meterMap[i]].VoltageAveraged/1000);
      setChannelCurrent((eChannel)(i+1),bMeter.chAverager[meterMap[i]].CurrentAveraged/1000);
    }*/
    setChannelVoltage(Channel_1,bMeter.chAverager[meterMap[0]].VoltageAveraged/1000);
    setChannelVoltage(Channel_2,bMeter.chAverager[meterMap[1]].VoltageAveraged/1000);
    setChannelVoltage(Channel_3,bMeter.chAverager[meterMap[2]].VoltageAveraged/1000);

    setChannelCurrent(Channel_1,bMeter.chAverager[meterMap[0]].CurrentAveraged/1000);
    setChannelCurrent(Channel_2,bMeter.chAverager[meterMap[1]].CurrentAveraged/1000);
    setChannelCurrent(Channel_3,bMeter.chAverager[meterMap[2]].CurrentAveraged/1000);
    
    //Update data

    //Check if there is any change relative to backend 


  }
  
}

void parseDataPC() {
  //ref: https://forum.arduino.cc/t/separating-a-comma-delimited-string/682868/6

  byte index = 0;
  pcptr = strtok(pcsbuffer,",");
  while(pcptr != NULL){        
    pcstrings[index] = pcptr;
    index++;
    pcptr = strtok(NULL, ",");
  }

  if(String(pcstrings[0])=="PC"){
    for(int i=0;i<3;i++){
      ScreenArr[i].tProp.numDev    = String(pcstrings[4*i+1]).toInt();
      ScreenArr[i].tProp.Dev1_Name = String(pcstrings[4*i+2]);
      ScreenArr[i].tProp.Dev2_Name = String(pcstrings[4*i+3]);
      ScreenArr[i].tProp.usbType   = String(pcstrings[4*i+4]).toInt();
    } 
  }

}

void mcuWriteAll(void){

  if(i2c_Semaphore != NULL){
    if(xSemaphoreTake(i2c_Semaphore,portMAX_DELAY) == pdTRUE){
      bMCU.writeAll();       
      xSemaphoreGive(i2c_Semaphore);
    } 
    else{
      HWSerial.println("Timeout to write I2C mcu");
    }
  }

}

void setCurrentLimitAll(float oclimit){
  
  if(i2c_Semaphore != NULL){
    if(xSemaphoreTake(i2c_Semaphore,portMAX_DELAY) == pdTRUE){
      bMeter.enableAlerts(false);
   
      for(int i=0; i<3; i++)
      {
        bMeter.setCurrentLimit(oclimit, true, i);
      }
      bMeter.enableAlerts(true);
      xSemaphoreGive(i2c_Semaphore);
    } 
    else{
      HWSerial.println("Timeout to write I2C PAC1943");
    }
  }  
}


void start_up_handler_task(void *pvParameters)
{
  while(1){
    if(startupactive)
    {
      for(int i=0; i<3; i++)
      {
        if(StartUpCounter[i]==1){
          bMCU.chArr[i].pwr_en = true;
          mcuWriteAll();
          HWSerial.println("Timeout CH "+ String(i+1));
        }        
        if(StartUpCounter[i]>0) StartUpCounter[i]--;
        
        ScreenArr[i].startup_cnt = StartUpCounter[i];        
      }

      if(StartUpCounter[0]<=0 && StartUpCounter[1]<=0 && StartUpCounter[2]<=0)  startupactive = false;

      vTaskDelay(STARTUP_TIMER_PERIOD/portTICK_RATE_MS);
    }
    else {
      HWSerial.println("End start up task");
      vTaskDelete(NULL);
    }
  }
}
