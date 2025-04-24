/*
Library to count milliseconds since start up, using Timer4 (8 bit) of STM8S MCUs

By ETPH 20160618, release under 

Parts of code are copyrighted by STMicroelectronics licensed under MCD-ST Liberty SW License Agreement V2
http://www.st.com/software_license_agreement_liberty_v2

This library must be used with STM8S_StdPeriph_Lib package which is provided free of charge by STMicroelectronics

Brief guide: 

- Two source files: stm8s_clk.c and stm8s_tim4.c must be added to project

- This code in tested on IAR. If using other compiler, the line "__IO uint32_t current_millis" might have to be put in the header file, and change to:
"volatile uint32_t current_millis"

- If your project is used with stm8s_it library, remember to comment out the function INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23) in stm8s_it.c
*/

#include "tim4millis.h"

__IO uint32_t current_millis = 0; //--IO: volatile read/write 


//this entire function using ST's code
void TIM4_init(void)
{
	//This call needs CLK library, consumes a lot of ROM
  //CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); //f_master = HSI = 16Mhz
	CLK->CKDIVR &= (uint8_t)(~CLK_CKDIVR_HSIDIV);
	CLK->CKDIVR |= 0x00;

	/* TIM4 configuration:
	- TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 128 so the TIM1 counter
	clock used is 16 MHz / 128 = 125 000 Hz
	- With 125 000 Hz we can generate time base:
	  max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
	  min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
	- In this example we need to generate a time base equal to 1 ms
	so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */

	/* Time base configuration */
	TIM4_TimeBaseInit(TIM4_PRESCALER_128, TIM4_PERIOD);
	/* Clear TIM4 update flag */
	TIM4_ClearFlag(TIM4_FLAG_UPDATE);
	/* Enable update interrupt */
	TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);

	/* enable interrupts */
	enableInterrupts();

	/* Enable TIM4 */
	TIM4_Cmd(ENABLE);
  
}

//Extracted from the definition in the stm8s_clk.c.
void CLK_DeInit(void)
{
  CLK->ICKR = CLK_ICKR_RESET_VALUE;
  CLK->ECKR = CLK_ECKR_RESET_VALUE;
  CLK->SWR  = CLK_SWR_RESET_VALUE;
  CLK->SWCR = CLK_SWCR_RESET_VALUE;
  CLK->CKDIVR = CLK_CKDIVR_RESET_VALUE;
  CLK->PCKENR1 = CLK_PCKENR1_RESET_VALUE;
  CLK->PCKENR2 = CLK_PCKENR2_RESET_VALUE;
  CLK->CSSR = CLK_CSSR_RESET_VALUE;
  CLK->CCOR = CLK_CCOR_RESET_VALUE;
  while ((CLK->CCOR & CLK_CCOR_CCOEN)!= 0)
  {}
  CLK->CCOR = CLK_CCOR_RESET_VALUE;
  CLK->HSITRIMR = CLK_HSITRIMR_RESET_VALUE;
  CLK->SWIMCCR = CLK_SWIMCCR_RESET_VALUE;
}


uint32_t millis(void)
{
	return current_millis;
}

//Interupt event, happen every 1 ms
#ifdef _RAISONANCE_
void TIM4_check_event(void) interrupt 23 {
#endif
#ifdef _COSMIC_
@far @interrupt void TIM4_check_event(void) {
#endif
  
	//increase 1, for millis() function
	current_millis ++;
	
	TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
}