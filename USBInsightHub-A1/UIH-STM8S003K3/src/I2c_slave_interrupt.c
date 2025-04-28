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
#include "I2c_slave_interrupt.h"

#define MAX_BUFFER  32
#define WHOAMI_ID 0x35
#define VERNUM 0x04

//registers
#define WHOAMI 0x10
#define VERSION 0x12 //add on version 3
#define CH1REG 0x20
#define CH2REG 0x21
#define CH3REG 0x22
#define CCSUM 0x23
#define AUXREG 0x24
#define MUXOECTR 0x26 //add on version 3

#define VEXTCC1L 0x30
#define VEXTCC1H 0x31
#define VEXTCC2L 0x32
#define VEXTCC2H 0x33
#define VHOSTCC1L 0x34
#define VHOSTCC1H 0x35
#define VHOSTCC2L 0x36
#define VHOSTCC2H 0x37



  u8 MessageBegin;
	u8 reg_address;

	bool muxoeReceived = FALSE; //i2C communication detected
	bool firstPowerFlag = TRUE; //first time power up flag for r24

	u8 r10_WHOAMI=0x10;
	u8 r12_VERSION= 0x12;
	/*u8 r20_CH1REG=0x54; //0x54 to power and data off; 0x5E to power and data on
	u8 r21_CH2REG=0x54; //0x54 to power and data off; 0x5E to power and data on
	u8 r22_CH3REG=0x54; //0x54 to power and data off; 0x5E to power and data on*/
	u8 r20_CH1REG=0x0; //0x54 to power and data off; 0x5E to power and data on
	u8 r21_CH2REG=0x0; //0x54 to power and data off; 0x5E to power and data on
	u8 r22_CH3REG=0x0; //0x54 to power and data off; 0x5E to power and data on	
	u8 r23_CCSUM=0x23;
	u8 r24_AUXREG=0x24;
	u8 r26_MUXOECTR=0x01; //Default is FUSB340 is enabled (USB3.0)
	
	u8 r30_VEXTCC1L=0x30;
	u8 r31_VEXTCC1H=0x31;
	u8 r32_VEXTCC2L=0x32;
	u8 r33_VEXTCC2H=0x33;

	u8 r34_VHOSTCC1L=0x34;
	u8 r35_VHOSTCC1H=0x35;
	u8 r36_VHOSTCC2L=0x36;
	u8 r37_VHOSTCC2H=0x37;



// ********************** Data link function ****************************
// * These functions must be modified according to your application neeeds *
// * See AN document for more precision
// **********************************************************************

	void I2C_transaction_begin(void)
	{
		MessageBegin = TRUE;
	}
	
	void I2C_transaction_end(void)
	{
		//Not used in this example
	}
	
	void I2C_byte_received(u8 u8_RxData)
	{
		if (MessageBegin == TRUE) {			
			reg_address= u8_RxData;
			MessageBegin = FALSE;
		}
    
		else {			
			Writable_Registers(reg_address, u8_RxData);
			reg_address++;
		}
	}
	
	u8 I2C_byte_write(void)
	{
		//if (u8_MyBuffp < &u8_My_Buffer[MAX_BUFFER])
		//	return *(u8_MyBuffp++);
		if(reg_address == WHOAMI) 				return WHOAMI_ID;
		else if(reg_address == VERSION) 	return VERNUM;
		else if(reg_address == CH1REG) 		return r20_CH1REG;
		else if(reg_address == CH2REG) 		return r21_CH2REG;
		else if(reg_address == CH3REG) 		return r22_CH3REG;
		else if(reg_address == CCSUM) 		return r23_CCSUM;
		else if(reg_address == AUXREG){
			firstPowerFlag = FALSE;
			return r24_AUXREG;
		} 		
		else if(reg_address == MUXOECTR) 	return r26_MUXOECTR;
		else if(reg_address == VEXTCC1L) 	return r30_VEXTCC1L;
		else if(reg_address == VEXTCC1H) 	return r31_VEXTCC1H;
		else if(reg_address == VEXTCC2L) 	return r32_VEXTCC2L;
		else if(reg_address == VEXTCC2H) 	return r33_VEXTCC2H;
		else if(reg_address == VHOSTCC1L) return r34_VHOSTCC1L;
		else if(reg_address == VHOSTCC1H) return r35_VHOSTCC1H;
		else if(reg_address == VHOSTCC2L) return r36_VHOSTCC2L;
		else if(reg_address == VHOSTCC2H) return r37_VHOSTCC2H;		
		else return 0x00;
	}

	void Writable_Registers(u8 reg_address, u8 u8_RxData)
	{
		if(reg_address==CH1REG){
			//mask the fault flag that is read only not to be overwritten by newdata
			r20_CH1REG = (r20_CH1REG | 0x7F ) & u8_RxData; 
		}
		else if(reg_address ==CH2REG){
			r21_CH2REG = (r21_CH2REG | 0x7F ) & u8_RxData;
		}
		else if(reg_address ==CH3REG){
			r22_CH3REG = (r22_CH3REG | 0x7F ) & u8_RxData;
		}
		else if(reg_address == MUXOECTR){
			r26_MUXOECTR = 0x01 & u8_RxData;
			muxoeReceived = TRUE;
		}
	}

// ********************** Data link interrupt handler *******************

#ifdef _RAISONANCE_
void I2C_Slave_check_event(void) interrupt 19 {
#endif
#ifdef _COSMIC_
@far @interrupt void I2C_Slave_check_event(void) {
#endif
	static u8 sr1;					
	static u8 sr2;
	static u8 sr3;
	
// save the I2C registers configuration
sr1 = I2C->SR1;
sr2 = I2C->SR2;
sr3 = I2C->SR3;

/* Communication error? */
  if (sr2 & (I2C_SR2_WUFH | I2C_SR2_OVR |I2C_SR2_ARLO |I2C_SR2_BERR))
  {		
    I2C->CR2|= I2C_CR2_STOP;  // stop communication - release the lines
    I2C->SR2= 0;					    // clear all error flags
	}
/* More bytes received ? */
  if ((sr1 & (I2C_SR1_RXNE | I2C_SR1_BTF)) == (I2C_SR1_RXNE | I2C_SR1_BTF))
  {
    I2C_byte_received(I2C->DR);
  }
/* Byte received ? */
  if (sr1 & I2C_SR1_RXNE)
  {
    I2C_byte_received(I2C->DR);
  }
/* NAK? (=end of slave transmit comm) */
  if (sr2 & I2C_SR2_AF)
  {	
    I2C->SR2 &= ~I2C_SR2_AF;	  // clear AF
		I2C_transaction_end();
	}
/* Stop bit from Master  (= end of slave receive comm) */
  if (sr1 & I2C_SR1_STOPF) 
  {
    I2C->CR2 |= I2C_CR2_ACK;	  // CR2 write to clear STOPF
		I2C_transaction_end();
	}
/* Slave address matched (= Start Comm) */
  if (sr1 & I2C_SR1_ADDR)
  {	 
		I2C_transaction_begin();
	}
/* More bytes to transmit ? */
  if ((sr1 & (I2C_SR1_TXE | I2C_SR1_BTF)) == (I2C_SR1_TXE | I2C_SR1_BTF))
  {
		I2C->DR = I2C_byte_write();
		reg_address++;
  }
/* Byte to transmit ? */
  if (sr1 & I2C_SR1_TXE)
  {
		I2C->DR = I2C_byte_write();
		reg_address++;		
  }	
	//GPIOD->ODR^=1;
	GPIOB->ODR^=1;
}


// ************************* I2C init Function  *************************

void Init_I2C (void)
{
	
	// sys clock /1
	CLK->CKDIVR = 0;                
	//Configure GPIO for I2C operation
	GPIOB->CR1 |= 0x30;
	GPIOB->DDR &= ~0x30;
	GPIOB->CR2 &= ~0x30;	
		
	/* Set I2C registers for 7Bits Address */
	I2C->CR1 |= 0x01;				        	// Enable I2C peripheral
	I2C->CR2 = 0x04;					      		// Enable I2C acknowledgement
	I2C->FREQR = 16; 					      	// Set I2C Freq value (16MHz)
	I2C->OARL = (SLAVE_ADDRESS << 1) ;	// set slave address to 0x51 (put 0xA2 for the register dues to7bit address) 
	I2C->OARH = 0x40;					      	// Set 7bit address mode

	I2C->ITR	= 0x07;					      // all I2C interrupt enable  
}

