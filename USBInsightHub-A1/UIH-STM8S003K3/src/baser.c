#include "baser.h"
#include "stm8s.h"
#include "stm8s_adc1.h"

extern u8 r10_WHOAMI;
extern u8 r12_VERSION;
extern u8 r20_CH1REG;
extern u8 r21_CH2REG;
extern u8 r22_CH3REG;
extern u8 r23_CCSUM;
extern u8 r24_AUXREG;
extern u8 r26_MUXOECTR;

extern u8 r30_VEXTCC1L;
extern u8 r31_VEXTCC1H;
extern u8 r32_VEXTCC2L;
extern u8 r33_VEXTCC2H;

extern u8 r34_VHOSTCC1L;
extern u8 r35_VHOSTCC1H;
extern u8 r36_VHOSTCC2L;
extern u8 r37_VHOSTCC2H;

u8 extdebcc1 = 0;
u8 extdebcc2 = 0;
u8 hostdebcc1 = 0;
u8 hostdebcc2 = 0;

extern bool firstPowerFlag;


void BaseR_GPIO_Init(void){
	
	GPIO_DeInit(GPIOA);
	GPIO_DeInit(GPIOB);
	GPIO_DeInit(GPIOC);
	GPIO_DeInit(GPIOD);
	GPIO_DeInit(GPIOE);
	GPIO_DeInit(GPIOF);
	
	GPIO_Init(BLINK, GPIO_MODE_OUT_PP_LOW_SLOW);
	
	GPIO_Init(CH1_PWR_EN, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(CH1_ILIM_H, GPIO_MODE_OUT_OD_HIZ_SLOW);
	GPIO_Init(CH1_ILIM_L, GPIO_MODE_OUT_OD_HIZ_SLOW);
	GPIO_Init(CH1_FAULT, GPIO_MODE_IN_PU_NO_IT);
	GPIO_Init(CH1_DATA_EN, GPIO_MODE_OUT_PP_LOW_SLOW);
	
	GPIO_Init(CH2_PWR_EN, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(CH2_ILIM_H, GPIO_MODE_OUT_OD_HIZ_SLOW);
	GPIO_Init(CH2_ILIM_L, GPIO_MODE_OUT_OD_HIZ_SLOW);
	GPIO_Init(CH2_FAULT, GPIO_MODE_IN_PU_NO_IT);
	GPIO_Init(CH2_DATA_EN, GPIO_MODE_OUT_PP_LOW_SLOW);

	GPIO_Init(CH3_PWR_EN, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(CH3_ILIM_H, GPIO_MODE_OUT_OD_HIZ_SLOW);
	GPIO_Init(CH3_ILIM_L, GPIO_MODE_OUT_OD_HIZ_SLOW);
	GPIO_Init(CH3_FAULT, GPIO_MODE_IN_PU_NO_IT);
	GPIO_Init(CH3_DATA_EN, GPIO_MODE_OUT_PP_LOW_SLOW);
	
	GPIO_Init(VEXT_PRESENT, GPIO_MODE_IN_FL_NO_IT);
	GPIO_Init(VHOST_PRESENT, GPIO_MODE_IN_FL_NO_IT);

	GPIO_Init(USB3_MUX_OE, GPIO_MODE_OUT_PP_HIGH_SLOW);
	GPIO_Init(USB3_MUX_SEL, GPIO_MODE_OUT_PP_HIGH_SLOW);

	//Configure USB3 control signals
	GPIO_WriteHigh(USB3_MUX_OE);
	GPIO_WriteLow(USB3_MUX_SEL);

	//GPIO configuration for ADC
	GPIO_Init(VHOST_CC1_AIN0, GPIO_MODE_IN_FL_NO_IT);
	GPIO_Init(VHOST_CC2_AIN1, GPIO_MODE_IN_FL_NO_IT);
	GPIO_Init(VEXT_CC1_AIN2, GPIO_MODE_IN_FL_NO_IT);
	GPIO_Init(VEXT_CC2_AIN3, GPIO_MODE_IN_FL_NO_IT);
}

void Update_GPIO_from_I2CRegisters(void){

	//First version does not use Ignore flags. The master MCU is fully responsible 
	//Map register fields to GPIOs
	
	if(r20_CH1REG & ILIMH_MASK)GPIO_WriteLow(CH1_ILIM_H); 
	else GPIO_WriteHigh(CH1_ILIM_H); //Active Low
	if(r20_CH1REG & ILIML_MASK)GPIO_WriteLow(CH1_ILIM_L); 
	else GPIO_WriteHigh(CH1_ILIM_L); //Active Low  				
	if(r20_CH1REG & PWREN_MASK)GPIO_WriteHigh(CH1_PWR_EN); 
	else GPIO_WriteLow(CH1_PWR_EN); //Active High
	if(r20_CH1REG & DATAEN_MASK)GPIO_WriteLow(CH1_DATA_EN); 
	else GPIO_WriteHigh(CH1_DATA_EN); //Active Low
	if(GPIO_ReadInputPin(CH1_FAULT))r20_CH1REG &= 0x7F; 
	else r20_CH1REG |= 0x80; 
	
	if(r21_CH2REG & ILIMH_MASK)GPIO_WriteLow(CH2_ILIM_H); 
	else GPIO_WriteHigh(CH2_ILIM_H);
	if(r21_CH2REG & ILIML_MASK)GPIO_WriteLow(CH2_ILIM_L); 
	else GPIO_WriteHigh(CH2_ILIM_L);  				
	if(r21_CH2REG & PWREN_MASK)GPIO_WriteHigh(CH2_PWR_EN); 
	else GPIO_WriteLow(CH2_PWR_EN);
	if(r21_CH2REG & DATAEN_MASK)GPIO_WriteLow(CH2_DATA_EN); 
	else GPIO_WriteHigh(CH2_DATA_EN);
	if(GPIO_ReadInputPin(CH2_FAULT))r21_CH2REG &= 0x7F; 
	else r21_CH2REG |= 0x80; 		
	
	if(r22_CH3REG & ILIMH_MASK)GPIO_WriteLow(CH3_ILIM_H); 
	else GPIO_WriteHigh(CH3_ILIM_H);
	if(r22_CH3REG & ILIML_MASK)GPIO_WriteLow(CH3_ILIM_L); 
	else GPIO_WriteHigh(CH3_ILIM_L);  				
	if(r22_CH3REG & PWREN_MASK)GPIO_WriteHigh(CH3_PWR_EN); 
	else GPIO_WriteLow(CH3_PWR_EN);
	if(r22_CH3REG & DATAEN_MASK)GPIO_WriteLow(CH3_DATA_EN); 
	else GPIO_WriteHigh(CH3_DATA_EN);		
	if(GPIO_ReadInputPin(CH3_FAULT))r22_CH3REG &= 0x7F; 
	else r22_CH3REG |= 0x80; 
	
	//Read VHOST or VEXT present signals
	if(GPIO_ReadInputPin(VHOST_PRESENT))r24_AUXREG 	|= 0x01; 
	else r24_AUXREG &= 0xFE;  
	if(GPIO_ReadInputPin(VEXT_PRESENT))r24_AUXREG 	|= 0x02; 
	else r24_AUXREG &= 0xFD;
	//Read the current status of USB3_MUX_OE 
	if(GPIO_ReadInputPin(USB3_MUX_OE))r24_AUXREG 		|= 0x04; 
	else r24_AUXREG &= 0xFB;	
	//Read the direction selected
	if(GPIO_ReadInputPin(USB3_MUX_SEL))r24_AUXREG 	|= 0x08; 
	else r24_AUXREG &= 0xF7;
	//Check if is the first startup. Clear flag after first read
	if(firstPowerFlag) r24_AUXREG 	|= 0x10;
	else r24_AUXREG &= 0xEF;
	
}

void Update_CC_signals(void) {

	unsigned int ADC_Ext_CC1 = ADC_Read(Ext_CC1_pin);
	unsigned int ADC_Ext_CC2 = ADC_Read(Ext_CC2_pin);
	unsigned int ADC_Host_CC1 = ADC_Read(Host_CC1_pin);
	unsigned int ADC_Host_CC2 = ADC_Read(Host_CC2_pin);
	u8 ccsumtemp = 0xff;
	unsigned int ccActive = 0;
	
	//update I2C registers with the voltage values of CC pins
	r30_VEXTCC1L = ADC_Ext_CC1&(0xff);
	r31_VEXTCC1H = (ADC_Ext_CC1>>8)&(0xff);
	r32_VEXTCC2L = ADC_Ext_CC2&(0xff);
	r33_VEXTCC2H = (ADC_Ext_CC2>>8)&(0xff);

	r34_VHOSTCC1L = ADC_Host_CC1&(0xff);
	r35_VHOSTCC1H = (ADC_Host_CC1>>8)&(0xff);
	r36_VHOSTCC2L = ADC_Host_CC2&(0xff);
	r37_VHOSTCC2H = (ADC_Host_CC2>>8)&(0xff);
	
	if(ADC_Ext_CC1 <= NOPULLH && ADC_Ext_CC2 <= NOPULLH) ccsumtemp &=0xF0;
	else
	{
		if(ADC_Ext_CC1 > ADC_Ext_CC2){
			ccActive= ADC_Ext_CC1;
			ccsumtemp &=0xF7;
		}			
		else {
			ccActive=ADC_Ext_CC2;
			ccsumtemp &=0xFB;
		}
		
		if (ccActive >= DEFLTL && ccActive <= DEFLTH)ccsumtemp &= 0xFD;
		else if (ccActive >= _1A5L && ccActive <= _1A5H)ccsumtemp &= 0xFE;
		else if (ccActive >= _3A0L && ccActive <= _3A0H)ccsumtemp &= 0xFF;
		else ccsumtemp &= 0xFC;		
	}		

	if(ADC_Host_CC1 <= NOPULLH && ADC_Host_CC2 <= NOPULLH) ccsumtemp &=0x0F;
	else
	{
		if(ADC_Host_CC1 > ADC_Host_CC2){
			ccActive= ADC_Host_CC1;
			ccsumtemp &=0x7F;
		}			
		else {
			ccActive=ADC_Host_CC2;
			ccsumtemp &=0xBF;
		}
		
		if (ccActive >= DEFLTL && ccActive <= DEFLTH)ccsumtemp &= 0xDF;
		else if (ccActive >= _1A5L && ccActive <= _1A5H)ccsumtemp &= 0xEF;
		else if (ccActive >= _3A0L && ccActive <= _3A0H)ccsumtemp &= 0xFF;
		else ccsumtemp &= 0xCF;		
	}		

	r23_CCSUM=ccsumtemp;
	
}


unsigned int ADC_Read(ADC1_Channel_TypeDef ADC_Channel_Number){
   unsigned int result = 0;
	 ADC1_DeInit();
	 ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, 
							 ADC_Channel_Number, 
							 ADC1_PRESSEL_FCPU_D18, 
							 ADC1_EXTTRIG_TIM, 
							 DISABLE, 
							 ADC1_ALIGN_RIGHT, 
							 ADC1_SCHMITTTRIG_ALL, 
							 DISABLE);
	ADC1_Cmd(ENABLE);
	ADC1_StartConversion();
	while(ADC1_GetFlagStatus(ADC1_FLAG_EOC) == FALSE);
	result = ADC1_GetConversionValue();
	ADC1_ClearFlag(ADC1_FLAG_EOC);
	ADC1_DeInit();
	return result;
}

