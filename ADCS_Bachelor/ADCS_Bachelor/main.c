#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "header.h"
#include "registers.h"

/*Defines*/
#define USART_BAUDRATE 9600
#define UBRR_VALUE ((F_CPU / (BAUDRATE * 16UL)) - 1 )


int main(void)
{
	initUSART();
	spiInit();
	printString("\r\nStarting connection test, please wait...");
	printString("\r\nIf the program holds here, check your connections.");
	_delay_ms(1000);
	WhoAmICheck();
	
	uint8_t testbyte = 0x00;
	uint8_t testbyte2 = 0x00;
		
	testbyte = SPIreadByte(PIN_XG, CTRL_REG4);
	printString("\r\nReading data from CTRL_REG4 (Expecting 56): ");
	printByte(testbyte);
	printString("");
		
	testbyte2 = SPIreadByte(PIN_M, CTRL_REG3_M);
	printString("\r\nReading data from CTRL_REG3_M (Expecting 3): ");
	printByte(testbyte2);
	printString("");
		
	
	initMag();
	initGyro();
	while(1){
		
	}
}