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
	
	SPIwriteByte(PIN_XG, INT_GEN_CFG_XL, 0xA5);
	SPIwriteByte(PIN_XG, INT_GEN_THS_X_XL, 0xD7);
		
	testbyte = SPIreadByte(PIN_XG, INT_GEN_CFG_XL);
	printString("\r\nReading data from INT_GEN_CFG_XL (Expecting 165): ");
	printByte(testbyte);
	printString("");
		
	testbyte2 = SPIreadByte(PIN_XG, INT_GEN_THS_X_XL);
	printString("\r\nReading data from INT_GEN_THS_X_XL (Expecting 215): ");
	printByte(testbyte2);
	printString("");
		
	
	while(1){
		;
	}
}