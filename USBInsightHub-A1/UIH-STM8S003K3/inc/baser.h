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
#include "stm8s_adc1.h"

//define MCU digital IOs 
#define BLINK GPIOD,GPIO_PIN_6 //Check if UART is used

#define CH1_PWR_EN GPIOC,GPIO_PIN_5
#define CH1_ILIM_H GPIOC,GPIO_PIN_7
#define CH1_ILIM_L GPIOC,GPIO_PIN_6
#define CH1_FAULT GPIOD,GPIO_PIN_0
#define CH1_DATA_EN GPIOC,GPIO_PIN_2

#define CH2_PWR_EN GPIOD,GPIO_PIN_2
#define CH2_ILIM_H GPIOD,GPIO_PIN_4
#define CH2_ILIM_L GPIOD,GPIO_PIN_3
#define CH2_FAULT GPIOD,GPIO_PIN_7
#define CH2_DATA_EN GPIOC,GPIO_PIN_3

#define CH3_PWR_EN GPIOA,GPIO_PIN_1
#define CH3_ILIM_H GPIOA,GPIO_PIN_3
#define CH3_ILIM_L GPIOA,GPIO_PIN_2
#define CH3_FAULT GPIOF,GPIO_PIN_4
#define CH3_DATA_EN GPIOC,GPIO_PIN_4

#define VEXT_PRESENT GPIOC,GPIO_PIN_1
#define VHOST_PRESENT GPIOE,GPIO_PIN_5

#define USB3_MUX_OE GPIOB,GPIO_PIN_7  //Signal inverted by Q2
#define USB3_MUX_SEL GPIOB,GPIO_PIN_6

#define VHOST_CC1_AIN0 GPIOB,GPIO_PIN_0
#define VHOST_CC2_AIN1 GPIOB,GPIO_PIN_1
#define VEXT_CC1_AIN2 GPIOB,GPIO_PIN_2
#define VEXT_CC2_AIN3 GPIOB,GPIO_PIN_3


#define ILIMG_MASK 0x01
#define ILIMH_MASK 0x02
#define ILIML_MASK 0x04
#define PWRENG_MASK 0x08
#define PWREN_MASK 0x10
#define DATAENG_MASK 0x20
#define DATAEN_MASK 0x40
#define FAULT_MASK 0x80

#define Ext_CC1_pin ADC1_CHANNEL_2 
#define Ext_CC2_pin ADC1_CHANNEL_3
#define Host_CC1_pin ADC1_CHANNEL_0
#define Host_CC2_pin ADC1_CHANNEL_1

#define NOPULLL 0
#define NOPULLH 47
#define DEFLTL 78
#define DEFLTH 189
#define _1A5L 217
#define _1A5H 360
#define _3A0L 406
#define _3A0H 632



//************ BASE R External Functions *********************

void BaseR_GPIO_Init(void);
void Update_GPIO_from_I2CRegisters(void);
unsigned int ADC_Read(ADC1_Channel_TypeDef ADC_Channel_Number);
void Update_CC_signals(void);
