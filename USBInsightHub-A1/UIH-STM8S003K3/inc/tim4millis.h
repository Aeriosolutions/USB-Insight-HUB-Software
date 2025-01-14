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


#ifndef TIM4MILLIS_H_
#define TIM4MILLIS_H_

#include "stm8s.h"
#include "stm8s_tim4.h"

#define TIM4_PERIOD 124
//volatile uint32_t current_millis

void CLK_DeInit(void);
void TIM4_init(void);

#ifdef _RAISONANCE_
	void TIM4_check_event(void) interrupt 23;
#endif
#ifdef _COSMIC_ 
		@far @interrupt void TIM4_check_event(void);
#endif


uint32_t millis(void);

#endif
