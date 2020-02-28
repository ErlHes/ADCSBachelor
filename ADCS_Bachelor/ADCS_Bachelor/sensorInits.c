#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "header.h"
#include "registers.h"


/* --------------------- GYROSCOPE -------------------- */


void initGyro(void){

	//	reference value for digital high pass filter
	//REFERENCE_G = 0x00;
	SPIwriteByte(PIN_XG, REFERENCE_G, 0x00);
	
	/*	bit 7-5:	output data rate selection, activates gyroscope when written
		bit 4-3:	full-scale selection
		bit 2:		always 0, don't write
		bit 1-0:	bandwidth selection */
	//CTRL_REG1_G = 0x00;
	SPIwriteByte(PIN_XG, CTRL_REG1_G, 0b10000000);
	
	/*	bit 7-4:	always 0
		bit 3-2:	INT (interrupt) selection configuration
		bit 1-0:	OUT selection configuration	*/
	//CTRL_REG2_G = 0x00;
	SPIwriteByte(PIN_XG, CTRL_REG2_G, 0x00);
	
	/*	bit 7:		low power mode enable
		bit 6:		high-pass filter enable
		bit 5-4:	always 0
		bit 3-0:	high-pass filter cutoff frequency selection*/
	//CTRL_REG3_G = 0x00;
	SPIwriteByte(PIN_XG, CTRL_REG3_G, 0x00);
	
}


void interuptGyro(void){
	
}


void calibGyro(void){
	
}


/* --------------------- MAGNETOMETER -------------------- */


void initMag(void){
	
	uint8_t tempRegValue = 0x00;
	
	uint8_t	magEnable = 0x01;
	// mag scale can be 4, 8, 12, or 16
	uint8_t magScale = 4;
	// mag data rate can be 0-7
	// 0 = 0.625 Hz  4 = 10 Hz
	// 1 = 1.25 Hz   5 = 20 Hz
	// 2 = 2.5 Hz    6 = 40 Hz
	// 3 = 5 Hz      7 = 80 Hz
	uint8_t magSampleRate = 7;
	uint8_t magTempCompensationEnable = 0x00;
	// magPerformance can be any value between 0-3
	// 0 = Low power mode      2 = high performance
	// 1 = medium performance  3 = ultra-high performance
	uint8_t magXYPerformance = 3;
	uint8_t magZPerformance = 3;
	uint8_t magLowPowerEnable = 0x00;
	// magOperatingMode can be 0-2
	// 0 = continuous conversion
	// 1 = single-conversion
	// 2 = power down
	uint8_t magOperatingMode = 0;
	
	/*	CTRL_REG1_M
		bit 7:		temperature compensation enable
		bit 6-5:	X and Y axis operation mode selection
		bit 4-2:	output data rate selection
		bit 1:		enables higher data rates than 80Hz
		bit 0:		self-test enable */	
	//	Default CTRL_REG1_M = 0x10
	if(magTempCompensationEnable){tempRegValue = (1<<7);}
	tempRegValue |= (magXYPerformance & 0x3) << 5;
	tempRegValue |= (magSampleRate & 0x7) << 2;
	SPIwriteByte(PIN_M, CTRL_REG1_M, tempRegValue);
	
	/*	CTRL_REG2_M
		bit 6-5:	full scale configuration
		bit 3:		reboot memory content
		bit 2:		configuration registers and user register reset function
		rest:		always 0 */
	//	Default CTRL_REG2_M = 0x00
	tempRegValue = 0;
	switch (magScale){
		case 8:
		tempRegValue |= (0x1 << 5);
		break;
		case 12:
		tempRegValue |= (0x2 << 5);
		break;
		case 16:
		tempRegValue |= (0x3 << 5);
		break;
		// Otherwise we'll default to 4 gauss (00)
	}
	SPIwriteByte(PIN_M, CTRL_REG2_M, tempRegValue);
	
	/*	CTRL_REG3_M
		bit 7:		I2C disable
		bit 5:		low-power mode configuration
		bit 2:		SPI mode selection
		bit 1-0:	operation mode selection
		rest:		always 0 */
	//	Default CTRL_REG3_M = 0x03
	tempRegValue = 0;
	if(magLowPowerEnable){ tempRegValue = (1<<5);}
	tempRegValue |= (magOperatingMode & 0x3);
	SPIwriteByte(PIN_M, CTRL_REG3_M, tempRegValue);
	
	/*	CTRL_REG4_M
		bit 3-2:	Z-axis operation mode selection
			00:low-power mode, 01:medium performance
			10:high performance, 10:ultra-high performance
		bit 1:		Endian data selection
		rest:		always 0 */
	//	Default CTRL_REG4_M = 0x00
	tempRegValue = 0;
	tempRegValue = (magZPerformance & 0x3) << 2;
	SPIwriteByte(PIN_M, CTRL_REG4_M, tempRegValue);
	
	/*	CTRL_REG5_M
		bit 7:	fast read enable
		bit 6:	block data update for magnetic data
		rest:	always 0 */
	//	Default CTRL_REG5_M = 0x00
	tempRegValue = 0;
	SPIwriteByte(PIN_M, CTRL_REG5_M, tempRegValue);
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


/* --------------------- ACCELEROMETER -------------------- */

// This page is intentionally left empty :)