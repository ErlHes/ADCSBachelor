#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "header.h"
#include "registers.h"


uint8_t getFIFOSamples(){
	return (SPIreadByte(PIN_XG, FIFO_SRC) & 0x3F);
}

void setFIFO(uint8_t fifoMode, uint8_t fifoThs){
	// Limit threshold - 0x1F (31) is the maximum. If more than that was asked
	// limit it to the maximum.
	uint8_t threshold = fifoThs <= 0x1F ? fifoThs : 0x1F;
	SPIwriteByte(PIN_XG, FIFO_CTRL, ((fifoMode & 0x7) << 5) | (threshold & 0x1F));
}

void enableFIFO(uint8_t enable){
	uint8_t temp = SPIreadByte(PIN_XG, CTRL_REG9);
	if (enable) temp |= (1<<1);
	else temp &= ~(1<<1);
	SPIwriteByte(PIN_XG, CTRL_REG9, temp);
}

void sleepGyro(uint8_t enable){
	uint8_t temp = SPIreadByte(PIN_XG, CTRL_REG9);
	if (enable) temp |= (1<<6);
	else temp &= ~(1<<6);
	SPIwriteByte(PIN_XG, CTRL_REG9, temp);
}

uint8_t getMagIntSrc(){
	uint8_t intSrc = SPIreadByte(PIN_M, INT_SRC_M);
	
	// Check if the INT (interrupt active) bit is set
	if (intSrc & (1<<0)){
		return (intSrc & 0xFE);
	}
	
	return 0;
}

void configMagThs(uint16_t threshold){
	// Write high eight bits of [threshold] to INT_THS_H_M
	SPIwriteByte(PIN_M, INT_THS_H_M, ((threshold & 0x7F00) >> 8));
	// Write low eight bits of [threshold] to INT_THS_L_M
	SPIwriteByte(PIN_M, INT_THS_L_M, (threshold & 0x00FF));
}

void configMagInt(uint8_t generator, uint8_t activeLow, uint8_t latch){
	// Mask out non-generator bits (0-4)
	uint8_t config = (generator & 0xE0);
	// IEA bit is 0 for active-low, 1 for active-high.
	if (activeLow == 0) config |= (1<<2);
	// IEL bit is 0 for latched, 1 for not-latched
	if (!latch) config |= (1<<1);
	// As long as we have at least 1 generator, enable the interrupt
	if (generator != 0) config |= (1<<0);
	
	SPIwriteByte(PIN_M, INT_CFG_M, config);
}

uint8_t getGyroIntSrc(){
	uint8_t intSrc = SPIreadByte(PIN_XG, INT_GEN_SRC_G);
	
	// Check if the IA_G (interrupt active) bit is set
	if (intSrc & (1<<6))
	{
		return (intSrc & 0x3F);
	}
	
	return 0;
}

void configGyroInt(uint8_t generator, uint8_t aoi, uint8_t latch){
	// Use variables from accel_interrupt_generator, OR'd together to create
	// the [generator]value.
	uint8_t temp = generator;
	if (aoi) temp |= 0x80;
	if (latch) temp |= 0x40;
	SPIwriteByte(PIN_XG, INT_GEN_CFG_G, temp);
}

void configGyroThs(int16_t threshold, uint8_t axis, uint8_t duration, uint8_t wait){
	uint8_t buffer[2];
	buffer[0] = (threshold & 0x7F00) >> 8;
	buffer[1] = (threshold & 0x00FF);
	// Write threshold value to INT_GEN_THS_?H_G and  INT_GEN_THS_?L_G.
	// axis will be 0, 1, or 2 (x, y, z respectively)
	SPIwriteByte(PIN_XG, INT_GEN_THS_XH_G + (axis * 2), buffer[0]);
	SPIwriteByte(PIN_XG, INT_GEN_THS_XH_G + 1 + (axis * 2), buffer[1]);
	
	// Write duration and wait to INT_GEN_DUR_XL
	uint8_t temp;
	temp = (duration & 0x7F);
	if (wait) temp |= 0x80;
	SPIwriteByte(PIN_XG, INT_GEN_DUR_G, temp);
}

uint8_t getInactivity(){
	uint8_t temp = SPIreadByte(PIN_XG, STATUS_REG_0);
	temp &= (0x10);
	return temp;
}

void configInactivity(uint8_t duration, uint8_t threshold, uint8_t sleepOn){
	uint8_t temp = 0;
	
	temp = threshold & 0x7F;
	if (sleepOn) temp |= (1<<7);
	SPIwriteByte(PIN_XG, ACT_THS, temp);
	
	SPIwriteByte(PIN_XG, ACT_DUR, duration);
}

void configInt(uint8_t interrupt_select, uint8_t generator, uint8_t activeLow, uint8_t pushPull){
	// Write to INT1_CTRL or INT2_CTRL. [interrupt] should already be one of
	// those two values.
	// [generator] should be an OR'd list of values from the interrupt_generators enum
	SPIwriteByte(PIN_XG, interrupt_select, generator);
	
	// Configure CTRL_REG8
	uint8_t temp;
	temp = SPIreadByte(PIN_XG, CTRL_REG8);
	
	if (activeLow) temp |= (1<<5);
	else temp &= ~(1<<5);
	
	if (pushPull) temp &= ~(1<<4);
	else temp |= (1<<4);
	
	SPIwriteByte(PIN_XG, CTRL_REG8, temp);
}

int16_t readMag(uint8_t axis_address){
	int8_t temp[2]; // We'll read six bytes from the mag into temp
	SPIreadBytes(PIN_M, axis_address, temp, 2);
	int16_t m = (temp[1] << 8 | temp[0]);
	return m;
}

int16_t readGyro(uint8_t axis_address){
	int8_t temp[2];
	SPIreadBytes(PIN_XG, axis_address, temp, 2);
	int16_t g = (temp[1] << 8 | temp[0]);
	return g;
}