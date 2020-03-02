#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "header.h"
#include "registers.h"


// Sensor Sensitivity Constants
// Values set according to the typical specifications provided in
// table 3 of the LSM9DS1 datasheet. (pg 12)
#define SENSITIVITY_ACCELEROMETER_2  0.000061
#define SENSITIVITY_ACCELEROMETER_4  0.000122
#define SENSITIVITY_ACCELEROMETER_8  0.000244
#define SENSITIVITY_ACCELEROMETER_16 0.000732
#define SENSITIVITY_GYROSCOPE_245    0.00875
#define SENSITIVITY_GYROSCOPE_500    0.0175
#define SENSITIVITY_GYROSCOPE_2000   0.07
#define SENSITIVITY_MAGNETOMETER_4   0.00014
#define SENSITIVITY_MAGNETOMETER_8   0.00029
#define SENSITIVITY_MAGNETOMETER_12  0.00043
#define SENSITIVITY_MAGNETOMETER_16  0.00058



/* --------------------- GYROSCOPE -------------------- */
// scaling, choose:		0 = 245dps, 1 = 500dps, 3 = 2000dps	
volatile uint8_t gyroScale = 0;

void initGyro(void){
	
	uint8_t gyroEnableX = 1;	// 0 for off, 1 for on
	uint8_t gyroEnableZ = 1;	// 0 for off, 1 for on
	uint8_t gyroEnableY = 1;	// 0 for off, 1 for on
	
	
	// gyro sample rate [MHz]: choose value between 1-6
	// 1 = 14.9    4 = 238
	// 2 = 59.5    5 = 476
	// 3 = 119     6 = 952
	uint8_t gyroSampleRate = 6;
	// bandwith is dependent on scaling, choose value between 0-3
	uint8_t gyroBandwidth = 0;
	uint8_t gyroLowPowerEnable = 0;	// 0 for off, 1 for on
	uint8_t gyroHPFEnable = 0;	// 0 for off, 1 for on
	// HPF cutoff frequency depends on sample rate
	// choose value between 0-9
	uint8_t gyroHPFCutoff = 0;
	uint8_t gyroFlipX = 0;	// 0 for default, 1 for orientation inverted
	uint8_t gyroFlipZ = 0;	// ----
	uint8_t gyroFlipY = 0;	// ----
	uint8_t gyroOrientation = 0b00000000;	// 3-bit value
	uint8_t gyroLatchINT = 0;	// 0 for off, 1 for on
	
	uint8_t tempValue = 0;
	
	/*	CTRL_REG1_G
		bit 7-5:	output data rate selection, activates gyroscope when written
		bit 4-3:	full-scale selection
		bit 2:		always 0, don't write
		bit 1-0:	bandwidth selection */
	//	Default: CTRL_REG1_G = 0x00;
	tempValue = (gyroSampleRate & 0b00000111) << 5;
	tempValue |= (gyroScale & 0b00000011) << 3;
	tempValue |= (gyroBandwidth & 0b00000011);
	SPIwriteByte(PIN_XG, CTRL_REG1_G, tempValue);
	
	/*	CTRL_REG2_G
		bit 7-4:	always 0
		bit 3-2:	INT (interrupt) selection configuration
		bit 1-0:	OUT selection configuration	*/
	//	Default: CTRL_REG2_G = 0x00;
	SPIwriteByte(PIN_XG, CTRL_REG2_G, 0x00);
	
	/*	CTRL_REG3_G
		bit 7:		low power mode enable
		bit 6:		high-pass filter enable
		bit 5-4:	always 0
		bit 3-0:	high-pass filter cutoff frequency selection */
	//	Default: CTRL_REG3_G = 0x00;
	tempValue = 0;
	tempValue = (gyroLowPowerEnable & 0x01) << 7;
	tempValue |= (gyroHPFEnable & 0x01) << 6;
	tempValue |= (gyroHPFCutoff & 0b0000111);
	SPIwriteByte(PIN_XG, CTRL_REG3_G, tempValue);
	
	/*	CTRL_REG4	
		bit 5:		yaw axis (Z) output enable
		bit 4:		roll axis (Y) output enable
		bit 3:		pitch axis (X) output enable
		bit 1:		lached interrupt
		bit 0:		4D option on interrupt
		rest:		always 0 */
	//	Default: CTRL_REG4 = 0x00;
	tempValue = 0;
	tempValue = (gyroEnableZ & 0x01) << 5;
	tempValue |= (gyroEnableY & 0x01) << 4;
	tempValue |= (gyroEnableX & 0x01) << 3;
	tempValue |= (gyroLatchINT & 0x01) << 1;
	SPIwriteByte(PIN_XG, CTRL_REG4, tempValue);
	
	/*	ORIENT_CFG_G
		bit 7-6:	always 0
		bit 5:		pitch axis (X) angular rate sign
		bit 4:		roll axis (Y) angular rate sign
		bit 3:		yaw axis (Z) angular rate sign
		bit 2-0:	directional user orientation selection */	
	// default = ORIENT_CFG_G = 0x00;
	tempValue = 0;
	tempValue = (gyroFlipX & 0x01) << 5;
	tempValue |= (gyroFlipY & 0x01) << 4;
	tempValue |= (gyroFlipZ & 0x01) << 3;
	tempValue |= (gyroOrientation & 0b00000111);
	SPIwriteByte(PIN_XG, ORIENT_CFG_G, tempValue);
}

float calcGyro(int16_t gyro){
	// Return the gyro raw reading times our pre-calculated DPS / (ADC tick):
	float temp = 0;
		switch (gyroScale){
			case 0:
			temp = gyro * SENSITIVITY_GYROSCOPE_245;
			break;
			case 1:
			temp = gyro * SENSITIVITY_GYROSCOPE_500;
			break;
			case 3:
			temp = gyro *  SENSITIVITY_GYROSCOPE_2000;
			break;
		}
	return temp;
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