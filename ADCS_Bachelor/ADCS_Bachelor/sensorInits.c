#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "header.h"
#include "registers.h"



/* --------------------- GYROSCOPE -------------------- */

void initGyro(void){
	
	uint8_t gyroEnableX = 1;	// 0 for off, 1 for on
	uint8_t gyroEnableZ = 1;	// 0 for off, 1 for on
	uint8_t gyroEnableY = 1;	// 0 for off, 1 for on
	
	
	// gyro sample rate [Hz]: choose value between 1-6
	// 1 = 14.9    4 = 238
	// 2 = 59.5    5 = 476
	// 3 = 119     6 = 952
	uint8_t gyroSampleRate = 3;
	// bandwidth is dependent on scaling, choose value between 0-3
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
		bit 1:		latched interrupt
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

void calibrateGyro(void){
	// int32_t aBiasRawTemp[3] = {0, 0, 0};	 //Not yet implemented
	int32_t gBiasRawTemp[3] = {0, 0, 0};
		
	printf("Calibrating gyroscope, hold the device still\n");
	for(int i = 0; i<1000; i++){
		if(i % 100 == 0)printf(".");
		readGyro();
		gBiasRawTemp[0] += gx;
		gBiasRawTemp[1] += gy;
		gBiasRawTemp[2] += gz;
		_delay_ms(9);	// Wait for guaranteed new data.
	}
	printf("\n");
	gBiasRawX = gBiasRawTemp[0] / 1000;
	gBiasRawY = gBiasRawTemp[1] / 1000;
	gBiasRawZ = gBiasRawTemp[2] / 1000;

}

/* --------------------- MAGNETOMETER -------------------- */


void initMag(void){
	
	uint8_t tempRegValue = 0x00;
	
//	uint8_t	magEnable = 0x01;

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


void calibrateMag(void){
	int i, j;
	int16_t magMin[3] = {0,0,0};
	int16_t magMax[3] = {0,0,0}; 
	int16_t mBiasRaw[3] = {0,0,0};
		
	for (i=0; i<128; i++){
		while(!availableMag(3));
		readMag();
		int16_t magTemp[3] = {0, 0, 0};
		magTemp[0] = mx;
		magTemp[1] = my;
		magTemp[2] = mz;
		for (j=0; j<3; j++){
			if (magTemp[j] > magMax[j]) magMax[j] = magTemp[j];
			if (magTemp[j] < magMin[j]) magMin[j] = magTemp[j];
		}
	}
	for (j=0; j<3; j++){
		mBiasRaw[j] = (magMax[j] + magMin[j]) / 2;
		// mBias[j] = calcMag(mBiasRaw[j]);
		offsetMag(j, mBiasRaw[j]);
	}
}


void offsetMag(uint8_t axis, int16_t offset){
	/*	Offset values. This values acts on the magnetic output data values 
		in order to subtract the environmental offset */
	if (axis > 2) return;	// make sure we don't write to wrong address 
	uint8_t msb = (offset & 0xFF00) >> 8;
	uint8_t lsb = offset & 0x00FF;
	SPIwriteByte(PIN_M, OFFSET_X_REG_L_M + (2 * axis), lsb);
	SPIwriteByte(PIN_M, OFFSET_X_REG_H_M + (2 * axis), msb);
}



/* --------------------- ACCELEROMETER -------------------- */

void initAccel(void){
	//test code only for debugging, fix this later.
	SPIwriteByte(PIN_XG, CTRL_REG6_XL, 0b00011000); // set Accel scale to +-8 g.
}

void calibrateAccel(void){
	int32_t aBiasRawTemp[3] = {0, 0, 0};
	
	printf("Calibrating Accelerometer, please put the device on a stable, level surface.\n");
	for(int i = 0; i<1000; i++){
		if(i % 100 == 0)printf(".");
		readAccel();
		aBiasRawTemp[0] += ax;
		aBiasRawTemp[1] += ay;
		aBiasRawTemp[2] += az - (int16_t)(1./SENSITIVITY_ACCELEROMETER_8);
		_delay_ms(9);	// Wait for guaranteed new data.
	}
	printf("\n");
	aBiasRawX = aBiasRawTemp[0] / 1000;
	aBiasRawY = aBiasRawTemp[1] / 1000;
	aBiasRawZ = aBiasRawTemp[2] / 1000;

}