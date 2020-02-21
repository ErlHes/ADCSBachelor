/*
 * testspii.c
 *
 * Created: 20.02.2020 10:49:14
 * Author : Edvard
 */ 

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
	
    while (1) {
		uint8_t G_X_L = spiRead(PINB1, OUT_X_H_XL);
		printString("\r\nRecieved byte: ");	//Print out start
		printByte(G_X_L);					//?????
		printString("");					//print out end
		_delay_ms(1000);
    }
}

