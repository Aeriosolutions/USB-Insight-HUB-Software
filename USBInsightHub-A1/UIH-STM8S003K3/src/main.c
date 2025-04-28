/**
 *   USB Insight Hub
 *
 *   A USB supercharged interfacing tool for developers & tech enthusiasts 
 *   https://github.com/Aeriosolutions/USB-Insight-HUB-Software
 *
 *   Copyright (C) 2024 - 2025 Aeriosolutions
 *   Copyright (C) 2024 - 2025 JoDaSa

 * MIT License. Check full description on LICENSE file.
 **/

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "I2c_slave_interrupt.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/ 
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/

 /*
  * Example firmware main entry point.
  * Parameters: None
  * Retval . None
  */
	
void main(void)
{
	
	CLK->CKDIVR = 0;                // sys clock /1

	/* Init GPIO for I2C use */
	GPIOE->CR1 |= 0x06;
	GPIOE->DDR &= ~0x06;
	GPIOE->CR2 &= ~0x06;

	/* Initialise I2C for communication */
	Init_I2C();
	
	/* Enable general interrupts */
	enableInterrupts();
	
	/*Main Loop */
  while(1);
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
