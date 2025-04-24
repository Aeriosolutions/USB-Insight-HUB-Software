#ifndef __I2C_SLAVE_H
#define __I2C_SLAVE_H
#include "stm8s.h"


/********************** EXTERNAL FUNCTION **********************************/  
	void transaction_begin(void);
	void transaction_end(void);
	void byte_received(u8 u8_RxData);
	u8 byte_write(void);
	void Writable_Registers(u8 reg_address, u8 u8_RxData);	
	void Init_I2C(void);
	
	#ifdef _RAISONANCE_
		void I2C_Slave_check_event(void) interrupt 19;
	#endif
	#ifdef _COSMIC_ 
			@far @interrupt void I2C_Slave_check_event(void);
	#endif
	
	
/********************** I2C configuration variables *****************************/  
	
		/* Define Slave Address  -----------------------------------------------------*/
	
	#define SLAVE_ADDRESS 0x51
	

	
#endif /*__I2C_SLAVE_H*/


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
