/**
  ******************************************************************************
  * file main.c
	* BaseRMCU for STM8s. Serves as IO extender, USB3 CC and data muxer controller.
	* Comunicates with a main CPU via I2C
  * version V4
  * date 23/02/2025
  ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "I2c_slave_interrupt.h"
#include "tim4millis.h"
#include "baser.h"

#define BLINK_INTERVAL 100 //in ms
#define CCSCAN_INTERVAL 10 //in ms
#define IOSCAN_INTERVAL 1 //in ms
#define CCDEBOUNCE 3 //in CCSCAN interval cycles
#define FAULTDEB 2 //in IOSCAN INTERVALs
#define WAIT_MUXOE_AT_START 1000 //in ms wait ESP32 to send r26_MUXOECTR

extern u8 r20_CH1REG;
extern u8 r21_CH2REG;
extern u8 r22_CH3REG;
extern u8 r23_CCSUM;
extern u8 r26_MUXOECTR;

extern bool muxoeReceived; //muxoe instruction received

uint32_t currentTime = 0;
uint32_t lastBlink = 0;
uint32_t lastCCScan = 0;
uint32_t IOScan = 0;

u8 ccActive=0;
u8 prevccActive=4; //non valid value to force dobounce on start-up
u8 muxDebounce=0;

bool waitEnd = FALSE; //initial waiting is over


void main(void)
{

	//Configure GPIO
	BaseR_GPIO_Init();

	//Configure Timer 4 for system clock
	CLK_DeInit(); //
  TIM4_init(); //already setup f_Master = HSI/1 = 16MHz
	//CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, ENABLE); //Enable Peripheral Clock for ADC
	CLK->PCKENR2 |= 0x08; //Enable Peripheral Clock for ADC
	/* Initialise I2C for communication */
	Init_I2C();
	
	/* Enable general interrupts */
	enableInterrupts();
	
	while (1)
	{
    
		currentTime = millis();
		
		if(currentTime >= WAIT_MUXOE_AT_START || muxoeReceived) 
			waitEnd = TRUE;
			
    if(currentTime - lastBlink >= BLINK_INTERVAL)
    {
			lastBlink = currentTime;
      GPIO_WriteReverse(BLINK);      
    }
		
		if(currentTime - lastCCScan >= CCSCAN_INTERVAL)
		{
			lastCCScan = currentTime;
			Update_CC_signals();
			//if CC1 is active, MUX sel=0; if CC2 is active, MUX sel=1
			if((r23_CCSUM & 0xC0)== 0x00 || (r23_CCSUM & 0xC0)== 0xC0) ccActive=0;
			if((r23_CCSUM & 0xC0)== 0x40) ccActive = 1;
			if((r23_CCSUM & 0xC0)== 0x80) ccActive = 2;
			//debounce cycle for activation
			if(ccActive != prevccActive){
				muxDebounce++;
				if(muxDebounce > CCDEBOUNCE){
					muxDebounce = 0;
					prevccActive = ccActive;
				}				
			}
			
			if(prevccActive == 0) 
				GPIO_WriteLow(USB3_MUX_OE);
			else {
				if(waitEnd && ((r26_MUXOECTR & 0x01)==0x01))
					GPIO_WriteHigh(USB3_MUX_OE); //Signal inverted by Q2
				else
					GPIO_WriteLow(USB3_MUX_OE);
					
				if(prevccActive == 1){					 
					 GPIO_WriteLow(USB3_MUX_SEL);
				}
				if(prevccActive == 2){					 
					 GPIO_WriteHigh(USB3_MUX_SEL);
				}						
			}
		}

    if(currentTime - IOScan >= IOSCAN_INTERVAL)
    { 
			IOScan = currentTime;		
      Update_GPIO_from_I2CRegisters();
		}			
			
	}
}


