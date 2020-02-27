#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "header.h"
#include "registers.h"


void initMag(void){
	
	/*	bit 7:		temperature compensation enable
		bit 6-5:	X and Y axis operation mode selection
		bit 4-2:	output data rate selection
		bit 1:		enables higher data rates than 80Hz
		bit 0:		self-test enable */	
	//CTRL_REG1_M = 0b00010000;
	SPIwriteByte(PIN_M, CTRL_REG1_M, 0b00010000);
	
	/*	bit 6-5:	full scale configuration
		bit 3:		reboot memory content
		bit 2:		configuration registers and user register reset function
		rest:		always 0 */
	//CTRL_REG2_M = 0x00;
	SPIwriteByte(PIN_M, CTRL_REG2_M, 0x00);
	
	/*	bit 7:		I2C disable
		bit 5:		low-power mode configuration
		bit 2:		SPI mode selection
		bit 1-0:	operation mode selection
		rest:		always 0 */
	//CTRL_REG3_M = 0b10000111;
	SPIwriteByte(PIN_M, CTRL_REG3_M, 0b10000111);
	
	/*	bit 3-2:	Z-axis operation mode selection
		bit 1:		Endian data selection
		rest:		always 0 */
	//CTRL_REG4_M = 0x00;
	SPIwriteByte(PIN_M, CTRL_REG4_M, 0x00);
	
	/*	bit 7:	fast read enable
		bit 6:	block data update for magnetic data
		rest:	always 0 */
	//CTRL_REG5_M = 0x00; 
	SPIwriteByte(PIN_M, CTRL_REG5_M, 0x00);
}

void interuptMag(void){
	/*	bit 7:	enable interrupt generation on X-axis
		bit 6:	enable interrupt generation on Y-axis
		bit 5:	enable interrupt generation on Z-axis
		bit 4-3:	always 0
		bit 2:	interrupt active configuration on INT_MAG
		bit 1:	latch interrupt request
		bit 0:	interrupt enable on INT_M pin */
	//INT_CFG_M = 0x00;
	SPIwriteByte(PIN_M, INT_CFG_M, 0x00);
}

void offsetMag(void){
	/*	Offset values. This values acts on the magnetic output data values 
		in order to subtract the environmental offset */
	//	X-axis:
	SPIwriteByte(PIN_M, OFFSET_X_REG_L_M, 0x00);
	SPIwriteByte(PIN_M, OFFSET_X_REG_H_M, 0x00);
	//	Y-axis:
	SPIwriteByte(PIN_M, OFFSET_Y_REG_L_M, 0x00);
	SPIwriteByte(PIN_M, OFFSET_Y_REG_H_M, 0x00);
	//	Z-axis:
	SPIwriteByte(PIN_M, OFFSET_Z_REG_L_M, 0x00);
	SPIwriteByte(PIN_M, OFFSET_Z_REG_H_M, 0x00);
}
